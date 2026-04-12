#ifndef CRAFTDIALOG_H
#define CRAFTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include "Personazh.h"

class CraftDialog : public QDialog {
    Q_OBJECT
public:
    explicit CraftDialog(const Personazh* p, QWidget* parent = nullptr, int imageMode = 0);

signals:
    void printClicked();

private slots:
    void onPrint();
    void onCancel();
    void saveImage();
    void changeImageMode(int index);

private:
    const Personazh* person;
    int currentImageMode;
    QLabel* imageLabel;
    QComboBox* imageModeCombo;
    QPushButton* saveImageBtn;
    QPixmap currentPixmap;

    void updateImage();
    QPixmap loadStaticImage() const;
    QPixmap loadRandomImage() const;
    QPixmap loadUniqueImage() const;
};

#endif