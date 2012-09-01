#include "pravidlo.h"

void pravidlo::pridaj_podmienky(QStringList podmienky){
    QString podmienka;
    foreach(podmienka,podmienky){
        this->podmienky.append(podmienka);
    }
}

void pravidlo::pridaj_akcie(QStringList akcie){
    QString akcia;
    foreach(akcia,akcie){
        this->akcie.append(akcia);
    }
}

void pravidlo::setnazov(QString meno){
    this->nazov=meno;
}

QLinkedList<QString> pravidlo::ziskaj_podmienky(){
    return this->podmienky;
}

QLinkedList<QString> pravidlo::ziskaj_akcie(){
    return this->akcie;
}

QString pravidlo::ziskaj_meno(){
    return this->nazov;
}

pravidlo::~pravidlo(){
}

void pravidlo::zmaz_akcie(){
    this->akcie.clear();
}

void pravidlo::zmaz_podmienky(){
    this->podmienky.clear();
}
