#include "MainWindow.h"
#include "CraftDialog.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QHeaderView>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {
    for (auto p : persons) delete p;
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QHBoxLayout* topLayout = new QHBoxLayout();
    btnLoad = new QPushButton("Загрузить TXT");
    btnLoadJSON = new QPushButton("Загрузить JSON");
    btnSaveJson = new QPushButton("Сохранить JSON");

    // Стили для кнопок
    btnLoad->setStyleSheet("QPushButton { background-color: #87CEEB; color: black; font-weight: bold; padding: 5px; border-radius: 3px; }");
    btnLoadJSON->setStyleSheet("QPushButton { background-color: #87CEEB; color: black; font-weight: bold; padding: 5px; border-radius: 3px; }");
    btnSaveJson->setStyleSheet("QPushButton { background-color: #3CB371; color: white; font-weight: bold; padding: 5px; border-radius: 3px; }");

    imageModeCombo = new QComboBox();
    imageModeCombo->addItems({"Статичные картинки", "Рандом из папок", "Уникальные"});

    topLayout->addWidget(btnLoad);
    topLayout->addWidget(btnLoadJSON);
    topLayout->addWidget(btnSaveJson);
    topLayout->addWidget(new QLabel("Режим картинок:"));
    topLayout->addWidget(imageModeCombo);
    topLayout->addStretch();

    table = new QTableWidget(0, 9);
    table->setHorizontalHeaderLabels({"Код","Имя","Стихия/Редкость","Здоровье","Мана/Урон","XP","Ш","К","С"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(table);
    setCentralWidget(central);

    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::loadFile);
    connect(btnLoadJSON, &QPushButton::clicked, this, &MainWindow::loadJSON);
    connect(btnSaveJson, &QPushButton::clicked, this, &MainWindow::saveToJson);
    connect(table, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(table, &QTableWidget::cellChanged, this, &MainWindow::editCell);

    resize(1000, 500);
    setWindowTitle("NPC Manager");
}

void MainWindow::loadFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите TXT файл", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;

    for (auto p : persons) delete p;
    persons.clear();

    parseFile(filename);
}

void MainWindow::parseFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        Personazh* p = createFromLine(line);
        if (p) persons.push_back(p);
    }
    refreshTable();
}

Personazh* MainWindow::createFromLine(const QString& line) {
    QStringList parts = line.split(',', Qt::SkipEmptyParts);
    if (parts.size() < 8) return nullptr;

    for (QString& part : parts) part = part.trimmed();

    int code = parts[0].toInt();
    QString name = parts[1];
    QString third = parts[2];
    int fourth = parts[3].toInt();
    int health = parts[4].toInt();
    Bronya armor(parts[5].toInt(), parts[6].toInt(), parts[7].toInt());

    int xp = 0, sh = 0, k = 0, c = 0;
    if (parts.size() >= 12) {
        xp = parts[8].toInt();
        sh = parts[9].toInt();
        k = parts[10].toInt();
        c = parts[11].toInt();
    }

    QString lowerThird = third.toLower();
    if (lowerThird.contains("огонь") || lowerThird.contains("вода") ||
        lowerThird.contains("земля") || lowerThird.contains("воздух") ||
        lowerThird.contains("молния") || lowerThird.contains("лёд")) {
        return new MagP(code, name, third, fourth, health, armor, xp, sh, k, c);
    } else {
        return new VragP(code, name, third, fourth, health, armor, xp, sh, k, c);
    }
}

void MainWindow::loadJSON() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите JSON файл", "", "JSON files (*.json)");
    if (filename.isEmpty()) return;

    for (auto p : persons) delete p;
    persons.clear();

    parseJSON(filename);
}

void MainWindow::parseJSON(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть JSON файл");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        QMessageBox::warning(this, "Ошибка", "JSON должен быть массивом");
        return;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue& val : arr) {
        QJsonObject obj = val.toObject();

        int code = obj["code"].toInt();
        QString name = obj["name"].toString();
        int health = obj["health"].toInt();
        Bronya armor(obj["helmet"].toInt(), obj["cuirass"].toInt(), obj["boots"].toInt());

        QString type = obj["type"].toString();
        int xp = obj["xp"].toInt();
        int sh = obj["sh"].toInt();
        int k = obj["k"].toInt();
        int c = obj["c"].toInt();

        if (type == "Mag") {
            QString element = obj["element"].toString();
            int mana = obj["mana"].toInt();
            persons.push_back(new MagP(code, name, element, mana, health, armor, xp, sh, k, c));
        } else {
            QString rarity = obj["rarity"].toString();
            int damage = obj["damage"].toInt();
            persons.push_back(new VragP(code, name, rarity, damage, health, armor, xp, sh, k, c));
        }
    }
    refreshTable();
}

void MainWindow::refreshTable() {
    table->setRowCount(static_cast<int>(persons.size()));
    for (size_t i = 0; i < persons.size(); ++i) {
        Personazh* p = persons[i];
        table->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(p->code)));
        table->setItem(static_cast<int>(i), 1, new QTableWidgetItem(p->name));
        table->setItem(static_cast<int>(i), 2, new QTableWidgetItem(p->getSpecial1()));
        table->setItem(static_cast<int>(i), 3, new QTableWidgetItem(QString::number(p->health)));
        table->setItem(static_cast<int>(i), 4, new QTableWidgetItem(QString::number(p->getSpecial2())));
        table->setItem(static_cast<int>(i), 5, new QTableWidgetItem(QString::number(p->getXP())));
        table->setItem(static_cast<int>(i), 6, new QTableWidgetItem(QString::number(p->getSH())));
        table->setItem(static_cast<int>(i), 7, new QTableWidgetItem(QString::number(p->getK())));
        table->setItem(static_cast<int>(i), 8, new QTableWidgetItem(QString::number(p->getC())));
    }
}

void MainWindow::onTableDoubleClicked(int row, int /*column*/) {
    if (row >= 0 && row < static_cast<int>(persons.size())) {
        Personazh* selectedPerson = persons[static_cast<size_t>(row)];
        CraftDialog* dlg = new CraftDialog(selectedPerson, this, imageModeCombo->currentIndex());
        dlg->setAttribute(Qt::WA_DeleteOnClose);

        connect(dlg, &CraftDialog::printClicked, this, [this, selectedPerson]() {
            removePersonazh(selectedPerson);
        });

        dlg->show();
    }
}

void MainWindow::editCell(int row, int column) {
    if (row < 0 || row >= static_cast<int>(persons.size())) return;

    QTableWidgetItem* item = table->item(row, column);
    if (!item) return;

    Personazh* p = persons[static_cast<size_t>(row)];
    QString newValue = item->text();

    switch (column) {
    case 0: p->code = newValue.toInt(); break;
    case 1: p->name = newValue; break;
    case 3: p->health = newValue.toInt(); break;
    }
}

void MainWindow::removePersonazh(Personazh* p) {
    auto it = std::find(persons.begin(), persons.end(), p);
    if (it != persons.end()) {
        delete *it;
        persons.erase(it);
        refreshTable();
    }
}

void MainWindow::saveToJson() {
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить JSON", "", "JSON (*.json)");
    if (filename.isEmpty()) return;

    QJsonArray arr;
    for (auto p : persons) {
        arr.append(p->toJson());
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson());
        QMessageBox::information(this, "Успех", "JSON файл сохранён");
    }
}