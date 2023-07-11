#include <QHBoxLayout>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QHeaderView>

#include "charactereditorwindow.h"
#include "mainwindow.h"

CharacterEditorWindow::CharacterEditorWindow(QWidget *parent) : QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);
    charTableView = new QTableView(this);
    charWindLayout = new QHBoxLayout(this);
    charWindLayout->addWidget(charTableView);
}

void CharacterEditorWindow::setDatabase(QSqlRelationalTableModel *charTable)
{
    charTable_p = charTable;
    charTableView->setModel(charTable_p);
    charTableView->resizeColumnsToContents();
    setMinimumWidth(charTableView->horizontalHeader()->length());
    charTable_p->select();
}
