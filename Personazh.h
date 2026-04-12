#ifndef PERSONAZH_H
#define PERSONAZH_H

#include <QString>
#include <QJsonObject>

struct Bronya {
    int helmet;
    int cuirass;
    int boots;
    Bronya(int h = 0, int c = 0, int b = 0) : helmet(h), cuirass(c), boots(b) {}
    QString toString() const;
};

class Personazh {
public:
    int code;
    QString name;
    int health;
    Bronya armor;

    Personazh(int c, const QString& n, int h, const Bronya& a);
    virtual ~Personazh() {}
    virtual void Craft(class QWidget* parent, int imageMode) const = 0;
    virtual QString type() const = 0;
    virtual QJsonObject toJson() const;
    virtual QString getSpecial1() const = 0;
    virtual int getSpecial2() const = 0;
    virtual int getXP() const = 0;
    virtual int getSH() const = 0;
    virtual int getK() const = 0;
    virtual int getC() const = 0;
};

class MagP : public Personazh {
public:
    QString element;
    int mana;
    int xp, sh, k, cVal;

    MagP(int code, const QString& name, const QString& elem, int manaVal,
         int health, const Bronya& armor, int xp, int sh, int k, int cVal);
    void Craft(QWidget* parent, int imageMode) const override;
    QString type() const override { return "Mag"; }
    QJsonObject toJson() const override;
    QString getSpecial1() const override { return element; }
    int getSpecial2() const override { return mana; }
    int getXP() const override { return xp; }
    int getSH() const override { return sh; }
    int getK() const override { return k; }
    int getC() const override { return cVal; }
};

class VragP : public Personazh {
public:
    QString rarity;
    int damage;
    int xp, sh, k, cVal;

    VragP(int code, const QString& name, const QString& rar, int dmg,
          int health, const Bronya& armor, int xp, int sh, int k, int cVal);
    void Craft(QWidget* parent, int imageMode) const override;
    QString type() const override { return "Vrag"; }
    QJsonObject toJson() const override;
    QString getSpecial1() const override { return rarity; }
    int getSpecial2() const override { return damage; }
    int getXP() const override { return xp; }
    int getSH() const override { return sh; }
    int getK() const override { return k; }
    int getC() const override { return cVal; }
};

#endif