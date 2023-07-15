#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QMap>

class QAction;
class QMenuBar;
class QMenu;
class QString;
class QWidget;

class MenuManager
{
public:
    explicit MenuManager(QMenuBar &menuBar, QWidget *parent = nullptr);
    ~MenuManager() {
        m_menus.clear();
        m_actions.clear();
    };

    QMenu   *addMenu(QString name);
    void     addAction(QAction *newAct, QString name, QMenu *menu);
    QMenu   *getMenu(QString name);
    QAction *getAction(QString name, QMenu *menu);

private:
    QMenuBar *m_menuBar;

    QMap<QString, QMenu *> m_menus;
    QMap<QString, QAction *> m_actions;

};

#endif // MENUMANAGER_H
