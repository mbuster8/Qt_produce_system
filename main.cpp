#include "okno.h"
#include <QtGui>
#include <QtCore>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    okno w;
    w.setWindowTitle("Produce system");
    w.show();

    return a.exec();
}
