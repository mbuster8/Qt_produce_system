#include "okno.h"
#include "ui_okno.h"
#include "pravidlo.h"
#include <QTextStream>
#include <QtGui>
#include <QtCore>

using namespace std;

okno::okno(QWidget *parent):QMainWindow(parent),ui(new Ui::okno){
        ui->setupUi(this);
        model_faktov = new QStringListModel(this);  //definica premennej typu QStringListModel
        model_pravidiel = new QStringListModel(this);
        model_aplikovatelnych_instancii = new QStringListModel(this);
        model_sprav = new QStringListModel(this);

        QStringList zoznam;
        //zoznam_pravidiel << "Michal je brat Matej" << "Michal je brat Krist�na" << "Danka je matka Michal";

        model_faktov->setStringList(zoznam);
        model_pravidiel->setStringList(zoznam);
        model_aplikovatelnych_instancii->setStringList(zoznam);
        model_sprav->setStringList(zoznam);

        //ui->comboBox->setModel(model_faktov);  //ten isty model - tie udaje, budu vsade, kde chcem, menim vzdy len model
        ui->okno_sprav->setModel(model_sprav);
        //ui->okno_sprav->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);
        ui->Okno_aplikovatelnych_instancii->setModel(model_aplikovatelnych_instancii);
        //ui->Okno_aplikovatelnych_instancii->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);
        ui->oknofaktov->setModel(model_faktov);
        ui->oknofaktov->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);
        ui->okno_pravidiel->setModel(model_pravidiel);
        ui->okno_pravidiel->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);
}
okno::~okno(){
    //delete model_faktov;              //tieto modely nemusim z pamati odstranovat, o to sa postara classa okno, lebo jej rodic je
    //delete model_pravidiel;
    //delete model_aplikovatelnych_instancii;
   //delete model_sprav;
    delete ui;
}
void okno::on_Pridaj_clicked(){ //stlacenie tlacidla pridat-> prida na dane miesto vo faktoch novy zaznam a nastavi na editaciu
    /*----------toto prida na koniec zoznamu novy zaznam---------------------
    int riadok = model_faktov->rowCount();  //mam tu pocet riadkov v zozname
    model_faktov->insertRows(riadok,1);    //na koniec teda vlozim prazny riadok - jeden

    QModelIndex index = model_faktov->index(riadok);   //do indexu vlozim index pridaneho riadku (kedze sa pocitalo od nula, index je spravny, netreba inkrementovat)
    ui->oknofaktov->setCurrentIndex(index);     //nastavim sa v zozname na dany index
    ui->oknofaktov->edit(index);                //a nastavim sa do edit modu -> upravujem dany riadok - moze tam pisat
    */

    int riadok = ui->oknofaktov->currentIndex().row()+1;  // vsun za aktualny riadok, na ktory som nastaveny v zozname faktov
    model_faktov->insertRows(riadok,1);    //za najdeny riadok teda vlozim prazny riadok - jeden

    QModelIndex index = model_faktov->index(riadok);   //do indexu vlozim index pridaneho riadku (kedze sa pocitalo od nula, index je spravny, netreba inkrementovat)
    ui->oknofaktov->setCurrentIndex(index);     //nastavim sa v zozname na dany index
    ui->oknofaktov->edit(index);                //a nastavim sa do edit modu -> upravujem dany riadok - moze tam pisat
}
void okno::on_vymaz_clicked(){  //zmaze aktualne vyznaceny zaznam vo faktoch
    model_faktov->removeRows(ui->oknofaktov->currentIndex().row(),1);
}
void okno::nacitaj(QString co){ //nacita fakty/pravidla zo zadaneho suboru, rozparsuje ich, naplni zoznam pravidiel tak ako ma byt, a vypise ich tam kde ma
    QString nazov_suboru = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Text Files(*.txt)"));
    QFile subor(nazov_suboru);

    if (!subor.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Fakty","Dany subor sa nepodarilo otvorit");
    }

    QTextStream vstup(&subor);
    QStringList zoznam;
    QString riadok;
    int pocitadlo=0;

    if (co == "otvor_fakty"){   //rozparsovanie faktov na vhodny tvar
        subor_faktov = nazov_suboru;
        while(!vstup.atEnd()){
            riadok = vstup.readLine();
            riadok.remove(QRegExp("[()\n]"));
            zoznam << riadok;
        }
        model_faktov->setStringList(zoznam);
        subor.flush();
        subor.close();
        return;
    }

    else if (co == "otvor_pravidla"){
        subor_pravidiel = nazov_suboru;
        pravidlo novepravidlo;
        while(!vstup.atEnd()){
            pocitadlo+=1;
            riadok = vstup.readLine();
            zoznam << riadok;       //nepozmeneny tvar riadku si musim zapamatat

            if (pocitadlo==1){      //som na nazve pravidla -> patricne rozparsovanie
                riadok.remove(QRegExp("[:\n]"));
                novepravidlo.setnazov(riadok);
            }
            else if (pocitadlo==2 || pocitadlo==3){  //som v casti podmienky alebo akcie -> rozparsujem ich
                riadok.replace(QRegExp("[\(+|\)+|AK\b|POTOM\b]"),"|");
                riadok.replace(QRegExp("(\\|+\\s+\\|+)|(\\|+)"),"|");
                QStringList rozbite = riadok.split(QRegExp("\\|"),QString::SkipEmptyParts);

                if (pocitadlo==2){  // som v casti podmienky -> patricne nastavim atributy prislusnej instancie classy
                    novepravidlo.pridaj_podmienky(rozbite);
                }
                else{               //som v casti akcie -> patricne nastavim atributy prislusnej instancie classy, a pridam nove pravidlo do zoznamu
                    novepravidlo.pridaj_akcie(rozbite);
                    this->pravidla.append(novepravidlo);
                }
            }

            else if (pocitadlo==4){ //musim vypraznit zoznamy, lebo ide nove pravidlo
                pocitadlo = 0;
                novepravidlo.zmaz_akcie();
                novepravidlo.zmaz_podmienky();
            }
        }

        subor.flush();
        subor.close();
        if (pocitadlo == 1 || pocitadlo == 2){
            zoznam.clear();
            zoznam << "Dany subor nesplna podmienky tvaru pravidiel";
        }
        model_pravidiel->setStringList(zoznam);
        return;
    }


    /*  //v subor_faktov mam celu cestu k suboru plus nazov suboru
    mod = new QStringListModel(this);
    QStringList nieco;
    nieco << subor_faktov;
    mod->setStringList(nieco);
    ui->listView_2->setModel(mod);
    */
}
void okno::on_otvor_pravidla_triggered(){ //spravne nastavim, kto je sender signalu a spracujem to vo funkcii nacitaj
    nacitaj(QObject::sender()->objectName());
}
void okno::on_otvor_fakty_triggered(){//spravne nastavim, kto je sender signalu a spracujem to vo funkcii nacitaj
    nacitaj(QObject::sender()->objectName());
}
void okno::on_krok_clicked(){ //spravim jeden krok -> najdem vsetky nove aplikovatelne instancie pravidla a zaradim ich do radu aplikovatelnych instancii
    krok();
}
QLinkedList<QHash <QString,QString> > okno::najdi_vhodne(QStringList slova_podmienky,QHash <QString,QString> premenne_pravidla_doteraz ){ //vracia zoznam map Qhash
    QLinkedList<QHash <QString,QString> > potencialne_instancie; //zoznam map(slovnikov)
    int pridaj = 0;
    QStringList fakty = model_faktov->stringList();
    QString fakt;
    QHash <QString,QString> namapovat_premenne;

    foreach(fakt,fakty){    //musim prebehnut vsetky fakty
        QStringList slova_faktu = fakt.split(" ",QString::SkipEmptyParts);
        if (slova_faktu.length()!=slova_podmienky.length()) continue; //pocet slov faktu je odlisny od poctu slov podmienky
        namapovat_premenne.clear();
        for (int i=0;i<premenne_pravidla_doteraz.count();i++){
            namapovat_premenne.insert(premenne_pravidla_doteraz.keys().at(i),premenne_pravidla_doteraz.values().at(i));
        }

        for (int pozicia_slova_vo_fakte=0;pozicia_slova_vo_fakte<slova_faktu.length();pozicia_slova_vo_fakte++){
            pridaj = 1;
            if (slova_podmienky.at(pozicia_slova_vo_fakte).at(0)=='?'){ //# slovo je premenna

                if ((namapovat_premenne[slova_podmienky.at(pozicia_slova_vo_fakte)]=="nic")&&
                     namapovat_premenne.values().contains(slova_faktu.at(pozicia_slova_vo_fakte))==false){

                    namapovat_premenne[slova_podmienky.at(pozicia_slova_vo_fakte)]=
                    slova_faktu.at(pozicia_slova_vo_fakte); //# namapujem novu premennu
                }
                else{//# musim skontrolovat, ci premenna faktu je rovna namapovanej premennej pravidla
                    if (namapovat_premenne[slova_podmienky.at(pozicia_slova_vo_fakte)]==
                        slova_faktu.at(pozicia_slova_vo_fakte)) continue;
                    else{
                        pridaj = 0;
                        break; //# premenne nie su identicke -> fakt nevyhovuje
                    }
                }
            }
            else if (slova_podmienky.at(pozicia_slova_vo_fakte)==slova_faktu.at(pozicia_slova_vo_fakte)) continue;
            else{
                pridaj =0;
                break;//#slovo faktu nie je premenna a nezhoduje sa so slovom v podmienke pravidla -> ideme na dalsi fakt
            }
        }
        if (namapovat_premenne.values().contains("nic") || (pridaj == 0)) continue; //#nie su namapovane vsetky doterajsie premenne, alebo nie su identicke premenne -> fakt nevyhovuje
        else  potencialne_instancie.append(namapovat_premenne);
    }
    return potencialne_instancie;
}
void okno::krok(){
    pravidlo aktualne_pravidlo;
    foreach(aktualne_pravidlo,pravidla){
        QLinkedList<QHash <QString,QString> > potencialne;   //zoznam slovnikov
        QHash <QString,QString> premenne_pravidla_doteraz,potencialna;
        QString podmienka,slovo_podmienky;

        foreach(podmienka,aktualne_pravidlo.ziskaj_podmienky()){
            QStringList slova_podmienky = podmienka.split(" ",QString::SkipEmptyParts);
            if (slova_podmienky.contains("<>")) break;  //#uz mam osetrene v predchadzajucom kode ze ziadne mapovane premenne sa nemozu rovnat, toto je posledna podmienka -> vsetky potencialne mozem testovat

            foreach(slovo_podmienky,slova_podmienky){
                if ((slovo_podmienky.at(0)=='?') & (premenne_pravidla_doteraz.keys().contains(slovo_podmienky)==false)){
                    premenne_pravidla_doteraz[slovo_podmienky] = "nic";
                    if (podmienka!=aktualne_pravidlo.ziskaj_podmienky().first()){
                        QLinkedList<QHash <QString,QString> > new_potencialne;
                        foreach(potencialna,potencialne){
                            potencialna[slovo_podmienky]="nic";
                            new_potencialne.append(potencialna);
                        }
                        potencialne.swap(new_potencialne);
                    }
                }
            }

            if (podmienka == aktualne_pravidlo.ziskaj_podmienky().first())potencialne.append(premenne_pravidla_doteraz);
            QLinkedList<QHash <QString,QString> > nove_potencialne;
            foreach(potencialna,potencialne){
                nove_potencialne+=najdi_vhodne(slova_podmienky,potencialna);//najdi_vhode vracia zoznam map
            }
            potencialne.swap(nove_potencialne);
        }
        filtruj_potencialne_instancie(aktualne_pravidlo,potencialne);   //#mam ziskane vsetky instancie pre 1 pravidlo, treba zlozit akcie pricom nahradit premenne
    }
    vykonaj_akcie_prvej_instancie_v_rade();
}
QString okno::nahrad(QString text,QHash <QString,QString> slovnik){
    for (int i=0;i<slovnik.values().length();i++){
        text = text.replace(slovnik.keys().at(i),slovnik.values().at(i));
    }
    return text;
}
void okno::filtruj_potencialne_instancie(pravidlo pravidielko, QLinkedList<QHash <QString,QString> > potencialne){
    QStringList slova_akcie;
    QStringList fakty = model_faktov->stringList();
    QString vykonat,akcia,nahradena_akcia;
    QHash <QString,QString> potencialna;
    foreach(potencialna,potencialne){
        foreach(akcia, pravidielko.ziskaj_akcie()){
            nahradena_akcia = nahrad(akcia,potencialna);    //tu mam presne znenie akcie (napriklad: pridaj Michal je brat Mato)
            nahradena_akcia.push_front(pravidielko.ziskaj_meno() + ":");    //na zaciatok akcie dam rodica akcie - nazov pravidla, z ktoreho pochadza
            if (pridane_v_minulosti.contains(nahradena_akcia)==false){
                slova_akcie = nahradena_akcia.split(" ",QString::SkipEmptyParts);
                QStringList dvojslovo = slova_akcie.at(0).split(":"); //prve slovo je typu Nazov:typ_akcie - musim ho rozdelit
                slova_akcie.pop_front();          //tymto vyhodim cele dvojslovo!!! - rodica aj typ akcie
                vykonat = dvojslovo[1];       //vo vykonat je nazov operacie -> pridaj/odstran/sprava alebo podobne
                akcia = slova_akcie.join(" ");

                if (fakty.contains(akcia)==false && (vykonat == "pridaj")){  //ak vo faktoch nie je dany fakt, pridam ho tam
                    zarad_do_radu_aplikovatelnych_instancii(vykonat,slova_akcie,pravidielko.ziskaj_meno());
                }
                else if (fakty.contains(akcia) && (vykonat == "vymaz")){  //ak vo faktoch je dany fakt, vymazem ho
                    zarad_do_radu_aplikovatelnych_instancii(vykonat,slova_akcie,pravidielko.ziskaj_meno());
                }
                else if (vykonat == "sprava"){
                    zarad_do_radu_aplikovatelnych_instancii(vykonat,slova_akcie,pravidielko.ziskaj_meno());
                }
            }
        }
    }

    if (model_faktov->rowCount()>10){
        slova_akcie.append("ahojte");
    }
}
void okno::zarad_do_radu_aplikovatelnych_instancii(QString vykonat,QStringList slova_akcie,QString meno_pravidla){
    if (vykonat == "sprava" && model_sprav->stringList().contains(slova_akcie.join(" "))==true)return;
    slova_akcie.push_front(vykonat);
    slova_akcie.push_front(meno_pravidla + ":");
    if (model_aplikovatelnych_instancii->stringList().contains(slova_akcie.join(" "))) return;
    pridane_v_minulosti.append(slova_akcie.join(" "));  //pridane_v_minulosti ma tvar: Rodic:prikaz (slova akcie bez prikazu)
    model_aplikovatelnych_instancii->insertRow(model_aplikovatelnych_instancii->rowCount());
    model_aplikovatelnych_instancii->setData(model_aplikovatelnych_instancii->index(model_aplikovatelnych_instancii->rowCount()-1),slova_akcie.join(" "));
}
QStringList okno::vrat_zoznam_pridanych_instancii_v_minulosti(){
                return this->pridane_v_minulosti;
}
int okno::vykonaj_akcie_prvej_instancie_v_rade(){    // #uz mam odiltrovane vhodne instancie, rovno vykonavam
    int vykonal = 0;
    if (model_aplikovatelnych_instancii->rowCount()>0){
        vykonal = 1;
        QStringList fakty = model_faktov->stringList();
        QStringList slova_prveho = model_aplikovatelnych_instancii->stringList().at(0).split(" ",QString::SkipEmptyParts);
        QString vykonaj = slova_prveho.at(1);
        QString rodic = slova_prveho.at(0);
        rodic.remove(":");
        slova_prveho.pop_front();   //potrebujem dat prec nazov pravidla a aj nazov akcie
        slova_prveho.pop_front();

        if (vykonaj=="pridaj"){
            model_faktov->insertRow(model_faktov->rowCount());
            model_faktov->setData(model_faktov->index(model_faktov->rowCount()-1),slova_prveho.join(" "));
        }
        else if (vykonaj == "vymaz"){
            fakty.removeAt(fakty.indexOf(slova_prveho.join(" ")));
            model_faktov->setStringList(fakty);
        }
        else if (vykonaj == "sprava"){
            model_sprav->insertRow(model_sprav->rowCount());
            model_sprav->setData(model_sprav->index(model_sprav->rowCount()-1),slova_prveho.join(" "));
        }
        model_aplikovatelnych_instancii->removeRow(0);

        if (model_aplikovatelnych_instancii->rowCount()>0){
            slova_prveho = model_aplikovatelnych_instancii->stringList().at(0).split(" ");
            QString novy_rodic;
            novy_rodic = slova_prveho.at(0).split(":").at(0);
            while (novy_rodic==rodic && slova_prveho.at(1)=="sprava"){
                slova_prveho.pop_front();
                slova_prveho.pop_front();
                model_sprav->insertRow(model_sprav->rowCount());
                model_sprav->setData(model_sprav->index(model_sprav->rowCount()-1),slova_prveho.join(" "));
                model_aplikovatelnych_instancii->removeRow(0);
                if (model_aplikovatelnych_instancii->rowCount()==0)break;

                slova_prveho = model_aplikovatelnych_instancii->stringList().at(0).split(" ");
                novy_rodic = slova_prveho.at(0).split(":").at(0);
            }
        }
    }

    return vykonal;
}
void okno::on_cele_clicked(){
    while (true){
        krok();
        if (vykonaj_akcie_prvej_instancie_v_rade()==0)break;
    }
}
void okno::on_reset_clicked(){
    QStringList zoznam;
    subor_faktov.clear();
    subor_pravidiel.clear();
    model_faktov->setStringList(zoznam);
    model_pravidiel->setStringList(zoznam);
    model_aplikovatelnych_instancii->setStringList(zoznam);
    model_sprav->setStringList(zoznam);
}
