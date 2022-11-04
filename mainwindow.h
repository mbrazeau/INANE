#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void createMainWindow();
    void createMenus();

    QPushButton *m_tutbutton;


signals:

};

#endif // MAINWINDOW_H
