#include <QApplication>

#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication    Inane(argc, argv);
    MainWindow      mw;

    mw.show();

    return Inane.exec();
}
