#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QTextEdit>
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>

#include "charactereditorwindow.h"
#include "mainwindow.h"

CharacterEditorWindow::CharacterEditorWindow(QWidget *parent) : QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);
    charTableView = new QTableView(this);
    charWindLayout = new QHBoxLayout(this);
    charWindLayout->addWidget(charTableView);

    charTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    charTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    charTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    charTable_p = nullptr;
}

void CharacterEditorWindow::setCharTable(QSqlRelationalTableModel *charTable, QSqlRelationalTableModel *statesTable)
{
    charTable_p = charTable;
    charTableView->setModel(charTable_p);
    charTableView->resizeColumnsToContents();
    setMinimumWidth(charTableView->horizontalHeader()->length());
    charTable_p->select();

    charTableView->setColumnHidden(0, true);

    statesTable_p = statesTable;

    initEditorArea();
}

void CharacterEditorWindow::initEditorArea()
{
    editorArea = new QWidget(this);

    QVBoxLayout *editorLayout = new QVBoxLayout(editorArea);

    labelField = new QLineEdit(editorArea);
    statesTable = new QTableView(editorArea);
    sourceField = new QLineEdit(editorArea);
    descripField = new QTextEdit(editorArea);

    labelLabel = new QLabel(QString(tr("Character name:")), editorArea);
    statesLabel = new QLabel(QString(tr("States:")), editorArea);
    sourceLabel = new QLabel(QString(tr("Source:")), editorArea);
    descripLabel = new QLabel(QString(tr("Description:")), editorArea);

    labelLabel->setBuddy(labelField);
    statesLabel->setBuddy(statesTable);
    sourceLabel->setBuddy(sourceField);
    descripLabel->setBuddy(descripField);

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(charTable_p);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(labelField, charTable_p->fieldIndex("label"));

    // Set up the states table:
    statesTable->setModel(statesTable_p);
    statesTable->setColumnHidden(0, true);
    statesTable->setColumnHidden(1, true);
    statesTable_p->select();

    mapper->toFirst();
    connect(charTableView, &QTableView::clicked, this, &CharacterEditorWindow::onCharacterClicked);

    // Place in layout
    editorLayout->addWidget(labelLabel);
    editorLayout->addWidget(labelField);
    editorLayout->addWidget(statesLabel);
    editorLayout->addWidget(statesTable);
    editorLayout->addWidget(sourceLabel);
    editorLayout->addWidget(sourceField);
    editorLayout->addWidget(descripLabel);
    editorLayout->addWidget(descripField);

    editorArea->setLayout(editorLayout);
    charWindLayout->addWidget(editorArea);

    QString charID;
    charID = charTable_p->record(0).value(QString("char_id")).toString();
    statesTable_p->setFilter(QString("character = '%1'").arg(charID));
}

void CharacterEditorWindow::onCharacterClicked(const QModelIndex &index)
{
    mapper->setCurrentModelIndex(index);

    QString charID;
    charID = charTable_p->record(index.row()).value(QString("char_id")).toString();
    statesTable_p->setFilter(QString("character = '%1'").arg(charID));
}
