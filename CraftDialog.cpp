#include "CraftDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPainter>
#include <QRandomGenerator>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

CraftDialog::CraftDialog(const Personazh* p, QWidget* parent, int imageMode)
    : QDialog(parent), person(p), currentImageMode(imageMode) {

    setWindowTitle("Карточка персонажа");
    setModal(true);
    resize(500, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Верхняя панель
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Режим картинки:"));
    imageModeCombo = new QComboBox();
    imageModeCombo->addItems({"Статичная", "Случайная из папки", "Уникальная"});
    imageModeCombo->setCurrentIndex(currentImageMode);
    topLayout->addWidget(imageModeCombo);

    saveImageBtn = new QPushButton("Сохранить картинку");
    topLayout->addWidget(saveImageBtn);
    topLayout->addStretch();

    QGroupBox* infoGroup = new QGroupBox("Информация о персонаже");
    QVBoxLayout* groupLayout = new QVBoxLayout(infoGroup);

    if (auto mag = dynamic_cast<const MagP*>(person)) {
        QLabel* nameLabel = new QLabel("Имя мага: " + mag->name);
        QFont boldFont = nameLabel->font();
        boldFont.setBold(true);
        boldFont.setPointSize(12);
        nameLabel->setFont(boldFont);
        nameLabel->setStyleSheet("QLabel { color: black; }");

        QString elementLower = mag->element.toLower();
        QString elementColor;
        if (elementLower.contains("огонь")) {
            elementColor = "#FF4500";
        } else if (elementLower.contains("вода")) {
            elementColor = "#1E90FF";
        } else if (elementLower.contains("воздух")) {
            elementColor = "#87CEEB";
        } else if (elementLower.contains("земля")) {
            elementColor = "#8B4513";
        } else {
            elementColor = "black";
        }

        QLabel* elemLabel = new QLabel("Стихия: <span style='color: " + elementColor + "; font-weight: bold;'>" + mag->element + "</span>");
        elemLabel->setStyleSheet("QLabel { color: black; }");
        elemLabel->setTextFormat(Qt::RichText);

        QLabel* hpLabel = new QLabel("ХП: <span style='color: #008000; font-weight: bold; font-size: 14px;'>" + QString::number(mag->health) + "</span>");
        hpLabel->setStyleSheet("QLabel { color: black; }");
        hpLabel->setTextFormat(Qt::RichText);

        QLabel* manaL = new QLabel("Мана: <span style='color: #0000FF; font-weight: bold; font-size: 14px;'>" + QString::number(mag->mana) + "</span>");
        manaL->setStyleSheet("QLabel { color: black; }");
        manaL->setTextFormat(Qt::RichText);

        QLabel* armorLabel = new QLabel("Броня: " + mag->armor.toString());
        armorLabel->setStyleSheet("QLabel { color: black; }");

        groupLayout->addWidget(nameLabel);
        groupLayout->addWidget(elemLabel);
        groupLayout->addWidget(hpLabel);
        groupLayout->addWidget(manaL);
        groupLayout->addWidget(armorLabel);

    } else if (auto vrag = dynamic_cast<const VragP*>(person)) {
        QString rarityLower = vrag->rarity.toLower();
        QString nameColor;
        if (rarityLower.contains("легенда")) {
            nameColor = "#FFD700";
        } else if (rarityLower.contains("редкий")) {
            nameColor = "#4169E1";
        } else if (rarityLower.contains("обычный")) {
            nameColor = "#32CD32";
        } else {
            nameColor = "black";
        }

        QLabel* nameLabel = new QLabel("Имя врага: <span style='color: " + nameColor + "; font-weight: bold;'>" + vrag->name + "</span>");
        QFont boldFont = nameLabel->font();
        boldFont.setBold(true);
        boldFont.setPointSize(12);
        nameLabel->setFont(boldFont);
        nameLabel->setStyleSheet("QLabel { color: black; }");
        nameLabel->setTextFormat(Qt::RichText);

        QLabel* rarLabel = new QLabel("Редкость: " + vrag->rarity);
        rarLabel->setStyleSheet("QLabel { color: black; }");

        QLabel* hpLabel = new QLabel("ХП врага: <span style='color: #FF0000; font-weight: bold; font-size: 14px;'>" + QString::number(vrag->health) + "</span>");
        hpLabel->setStyleSheet("QLabel { color: black; }");
        hpLabel->setTextFormat(Qt::RichText);

        QLabel* dmgLabel = new QLabel("Урон: <span style='color: #8A2BE2; font-weight: bold;'>" + QString::number(vrag->damage) + "</span>");
        dmgLabel->setStyleSheet("QLabel { color: black; }");
        dmgLabel->setTextFormat(Qt::RichText);

        QLabel* armorLabel = new QLabel("Броня: " + vrag->armor.toString());
        armorLabel->setStyleSheet("QLabel { color: black; font-weight: bold; }");

        groupLayout->addWidget(nameLabel);
        groupLayout->addWidget(rarLabel);
        groupLayout->addWidget(hpLabel);
        groupLayout->addWidget(dmgLabel);
        groupLayout->addWidget(armorLabel);
    }

    imageLabel = new QLabel;
    imageLabel->setFixedSize(200, 200);
    imageLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("QLabel { background-color: #f0f0f0; }");

    QHBoxLayout* contentLayout = new QHBoxLayout;
    contentLayout->addWidget(infoGroup);
    contentLayout->addWidget(imageLabel);

    QPushButton* printBtn = new QPushButton("Печать");
    QPushButton* cancelBtn = new QPushButton("Отмена");
    printBtn->setFixedWidth(100);
    cancelBtn->setFixedWidth(100);

    printBtn->setStyleSheet("QPushButton { background-color: #4169E1; color: white; font-weight: bold; padding: 8px 15px; border-radius: 4px; }");
    cancelBtn->setStyleSheet("QPushButton { background-color: #DC143C; color: white; font-weight: bold; padding: 8px 15px; border-radius: 4px; }");

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(printBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(contentLayout);
    mainLayout->addLayout(btnLayout);

    connect(printBtn, &QPushButton::clicked, this, &CraftDialog::onPrint);
    connect(cancelBtn, &QPushButton::clicked, this, &CraftDialog::onCancel);
    connect(saveImageBtn, &QPushButton::clicked, this, &CraftDialog::saveImage);
    connect(imageModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CraftDialog::changeImageMode);

    updateImage();
}

void CraftDialog::changeImageMode(int index) {
    currentImageMode = index;
    updateImage();
}

void CraftDialog::updateImage() {
    switch (currentImageMode) {
    case 0: currentPixmap = loadStaticImage(); break;
    case 1: currentPixmap = loadRandomImage(); break;
    case 2: currentPixmap = loadUniqueImage(); break;
    default: currentPixmap = loadStaticImage();
    }
    imageLabel->setPixmap(currentPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QPixmap CraftDialog::loadStaticImage() const {
    QString path = dynamic_cast<const MagP*>(person) ? ":/images/mage_default" : ":/images/enemy_default";
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        pixmap = QPixmap(200, 200);
        pixmap.fill(dynamic_cast<const MagP*>(person) ? QColor(100, 150, 255) : QColor(255, 100, 100));
        QPainter painter(&pixmap);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, dynamic_cast<const MagP*>(person) ? "MAGE" : "ENEMY");
    }
    return pixmap;
}

QPixmap CraftDialog::loadRandomImage() const {
    QString folderPath = QCoreApplication::applicationDirPath() + "/";
    folderPath += dynamic_cast<const MagP*>(person) ? "mag_res" : "vrag_res";
    QDir dir(folderPath);
    QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
    QStringList files = dir.entryList(filters, QDir::Files);
    if (!files.isEmpty()) {
        return QPixmap(dir.filePath(files[QRandomGenerator::global()->bounded(files.size())]));
    }
    return loadStaticImage();
}

QPixmap CraftDialog::loadUniqueImage() const {
    QString basePath = QCoreApplication::applicationDirPath() + "/";
    basePath += dynamic_cast<const MagP*>(person) ? "mag_res/" : "vrag_res/";

    qDebug() << "=== ЗАГРУЗКА УНИКАЛЬНОЙ КАРТИНКИ ===";
    qDebug() << "Персонаж:" << person->name;
    qDebug() << "Папка поиска:" << basePath;

    QStringList possibleNames;
    possibleNames << person->name.toLower().replace(" ", "_") + ".png"
                  << QString::number(person->code) + ".png"
                  << person->getSpecial1().toLower() + ".png";

    qDebug() << "Ищем файлы:" << possibleNames;

    for (const QString& name : possibleNames) {
        QString fullPath = basePath + name;
        qDebug() << "Проверяю:" << fullPath;
        QFile file(fullPath);
        qDebug() << "  Файл существует?" << file.exists();
        QPixmap pixmap(fullPath);
        if (!pixmap.isNull()) {
            qDebug() << "  ✅ КАРТИНКА ЗАГРУЖЕНА!";
            return pixmap;
        } else {
            qDebug() << "  ❌ Не удалось загрузить";
        }
    }

    qDebug() << "❌ Картинка не найдена, использую заглушку";

    QPixmap pixmap(200, 200);
    pixmap.fill(Qt::lightGray);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, person->name);
    return pixmap;
}

void CraftDialog::saveImage() {
    if (currentPixmap.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Нет изображения для сохранения");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить изображение", person->name + ".png", "Images (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty() && currentPixmap.save(fileName)) {
        QMessageBox::information(this, "Успех", "Изображение сохранено");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение");
    }
}

void CraftDialog::onPrint() {
    emit printClicked();
    accept();
}

void CraftDialog::onCancel() {
    reject();
}