#include <QCoreApplication>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QPair>
#include <QWidget>
#include <QMainWindow>

#include "menumanager.h"

MenuManager::MenuManager(QMenuBar &menuBar, QWidget *parent)
{
    Q_UNUSED(parent);
    m_menuBar = &menuBar;
}

QMenu *MenuManager::addMenu(QString name)
{
    QMenu *newMenu = m_menuBar->addMenu(QString("&") + name);
    m_menus[name] = newMenu;
    return newMenu;
}

void MenuManager::addAction(QAction *newAct, QString name, QMenu *menu)
{
    menu->addAction(newAct);
//    m_actions[name] = newAct;
}

QMenu *MenuManager::getMenu(QString name)
{
    return m_menus[name];
}
