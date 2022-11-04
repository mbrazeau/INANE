#include <QApplication>
#include <QPushButton>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    setMinimumSize(500, 300);
    m_tutbutton = new QPushButton(this);
    m_tutbutton->setGeometry(10, 10, 80, 30);
    m_tutbutton->setText("Fuckoff.");
    m_tutbutton->setToolTip("You get the message?");
    m_tutbutton->setFont(QFont("Courier New"));
    m_tutbutton->show();

    connect(m_tutbutton, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));

}

void MainWindow::createMainWindow()
{

}

//void MainWindow::createMenus()
//{
//    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
//}
