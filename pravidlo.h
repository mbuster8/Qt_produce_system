#ifndef PRAVIDLO_H
#define PRAVIDLO_H

#include <QtCore>

class pravidlo{
    QString nazov;
    QLinkedList<QString> podmienky,akcie;
public:
    pravidlo(){}
    void setnazov(QString);
    void pridaj_podmienky(QStringList);
    void pridaj_akcie(QStringList);
    ~pravidlo();
    QLinkedList<QString> ziskaj_podmienky();
    QLinkedList<QString> ziskaj_akcie();
    QString ziskaj_meno();
    void zmaz_akcie();
    void zmaz_podmienky();

};

#endif // PRAVIDLO_H
