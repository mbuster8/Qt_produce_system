#ifndef OKNO_H
#define OKNO_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include "pravidlo.h"

namespace Ui{
    class okno;
}

class okno : public QMainWindow{
    Q_OBJECT

public:
    explicit okno(QWidget *parent = 0);
    ~okno();

    QLinkedList<QHash <QString,QString> > najdi_vhodne(QStringList,QHash <QString,QString>); //najde vhodne instancie pravidiel pre vsetky fakty
    QStringList vrat_zoznam_pridanych_instancii_v_minulosti();
    QString nahrad(QString text,QHash <QString,QString> slovnik);
    void filtruj_potencialne_instancie(pravidlo, QLinkedList<QHash <QString,QString> >);
    void krok();
    void nacitaj(QString);

private slots:
    void on_Pridaj_clicked();
    void on_vymaz_clicked();
    void on_otvor_pravidla_triggered();
    void on_otvor_fakty_triggered();
    void on_krok_clicked();
    void on_cele_clicked();
    void on_reset_clicked();

private:
    QLinkedList<pravidlo> pravidla;
    Ui::okno *ui;
    QStringListModel *model_faktov,*model_pravidiel,*model_aplikovatelnych_instancii,*model_sprav;   //deklaracia premennej typu QStringListModel
    QString subor_faktov;
    QString subor_pravidiel;
    QStringList pridane_v_minulosti;

    void zarad_do_radu_aplikovatelnych_instancii(QString,QStringList,QString);
    int vykonaj_akcie_prvej_instancie_v_rade();
};

#endif // OKNO_H
