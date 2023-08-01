#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication    Inane(argc, argv);

    /* Fusion style is platform-agnostic and eliminates some display
     * issues that occur on a Mac.*/
//    Inane.setStyle(QStyleFactory::create("Fusion"));

    MainWindow mw;
    mw.show();

    return Inane.exec();
}
