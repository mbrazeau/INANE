#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication    Inane(argc, argv);
    Inane.setStyle(QStyleFactory::create("Fusion"));
    MainWindow      mw;

    mw.show();

    return Inane.exec();
}
