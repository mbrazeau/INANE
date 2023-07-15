#include <QCryptographicHash>
#include <QDataWidgetMapper>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlError>
#include <QTableView>
#include <QTextEdit>

#include "charactereditorwindow.h"
#include "mainwindow.h"

CharacterEditorWindow::CharacterEditorWindow(QWidget *parent) : QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);
    charTableView = new QTableView(this);
    charWindLayout = new QGridLayout(this);
    charWindLayout->addWidget(charTableView, 0, 0, 8, 1);

    newChar = new QPushButton(tr("New character"), this);
    connect(newChar, &QPushButton::released, this, &CharacterEditorWindow::newCharacterAction);
    deleteChar = new QPushButton(tr("Delete character"), this);
    connect(deleteChar, &QPushButton::released, this, &CharacterEditorWindow::deleteCharAction);

    charWindLayout->addWidget(newChar, 8, 0);
    charWindLayout->addWidget(deleteChar, 9, 0);

    charTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    charTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    charTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    charTable_p = nullptr;

    setCharTable();

    initEditorArea();
}

void CharacterEditorWindow::setCharTable()
{
//    charTable_p = charTable;
    charTable_p = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    charTable_p->setTable("characters");
    charTableView->setModel(charTable_p);
    charTable_p->select();
    charTableView->resizeColumnsToContents();

    charTableView->setColumnHidden(0, true);
    charTableView->setColumnHidden(1, true);
    charTableView->setColumnHidden(3, true);
    charTableView->setColumnHidden(4, true);

    charTableView->selectRow(0);

    statesTable_p = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    statesTable_p->setTable("states");
}

void CharacterEditorWindow::initEditorArea()
{
    editorArea = new QWidget(this);

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
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(labelField, charTable_p->fieldIndex("label"));
    mapper->addMapping(sourceField, charTable_p->fieldIndex("source"));
    mapper->addMapping(descripField, charTable_p->fieldIndex("notes"));

    // Set up the states table:
    statesTable->setModel(statesTable_p);
    statesTable->setColumnHidden(0, true);
    statesTable->setColumnHidden(2, true);
    statesTable_p->select();

    mapper->toFirst();
    connect(charTableView, &QTableView::clicked, this, &CharacterEditorWindow::onCharacterClicked);

    // Place in layout
    charWindLayout->addWidget(labelLabel, 0, 1);
    charWindLayout->addWidget(labelField, 1, 1);
    charWindLayout->addWidget(statesLabel, 2, 1);
    charWindLayout->addWidget(statesTable, 3, 1);
    charWindLayout->addWidget(sourceLabel, 4, 1);
    charWindLayout->addWidget(sourceField, 5, 1);
    charWindLayout->addWidget(descripLabel, 6, 1);
    charWindLayout->addWidget(descripField, 7, 1);

    submitButton = new QPushButton(tr("Submit"), this);
    charWindLayout->addWidget(submitButton, 8, 1);
    connect(submitButton, &QPushButton::released, this, &CharacterEditorWindow::commitCharChange);

//    charTable_p->row

//    editorArea->setLayout(editorLayout);
//    charWindLayout->addWidget(editorArea, 0, 1);

    QString charID;
    charID = charTable_p->record(0).value(QString("char_id")).toString();
    statesTable_p->setFilter(QString("states.character = '%1'").arg(charID));
}

void CharacterEditorWindow::deleteCharAction()
{
    QMessageBox checkDelete;
    checkDelete.setIcon(QMessageBox::Critical);
    checkDelete.setText("Are you sure you want to delete this character?");
    checkDelete.setInformativeText("Deleting characters is not best practice. If you want to exclude a character from analyses, you should toggle its inclusion status.");
    checkDelete.setStandardButtons(QMessageBox::Abort);
    checkDelete.setDefaultButton(QMessageBox::Abort);

    int ret = checkDelete.exec();

    int row;
    switch (ret) {
    case QMessageBox::Abort:
        break;
    case QMessageBox::Ignore:
        row = charTableView->currentIndex().row();
        charTable_p->removeRow(row);
        charTable_p->select();
        if (row < charTable_p->rowCount()) {
            charTableView->selectRow(row);
        } else {
            charTableView->selectRow(row - 1);
        }
        onCharacterClicked(charTableView->currentIndex());
        break;
    default:
        break;
    }
}

void CharacterEditorWindow::newCharacterAction()
{
    mapper->toLast();
    int row = mapper->currentIndex();
    qDebug() << "Row: " << row;

    statesTable_p->setFilter("x"); // Temporarily disable filter

    QSqlQuery query;
    if(!query.exec("INSERT INTO characters (char_GUUID, label) VALUES (NULL, 'new character')")) {
        qDebug() << "Failed to create new character!";
    }

    mapper->toLast();

    int newID;

    charTable_p->select();
    mapper->setCurrentIndex(row + 1);

    newID = charTable_p->record(row+1).value(QString("char_id")).toInt();
    query.exec(QString("INSERT INTO states (character, label) VALUES (%1, 'state_1')").arg(newID));
    query.exec(QString("INSERT INTO states (character, label) VALUES (%1, 'state_2')").arg(newID));
    query.exec(QString("INSERT INTO states (character, label) VALUES (%1, 'missing')").arg(newID));
    query.exec(QString("INSERT INTO states (character, label) VALUES (%1, 'inapplicable')").arg(newID));

    statesTable_p->setFilter(QString("character = %1").arg(newID)); //Re-enable filter on new characters
//    mapper->submit();

    charTableView->setEnabled(false);
    newChar->setEnabled(false);
    deleteChar->setEnabled(false);

    charTable_p->select();
    statesTable_p->select();

    mapper->toLast();
}

void CharacterEditorWindow::commitCharChange()
{
    mapper->submit();

    if (charTableView->isEnabled() == false) {
        mapper->toLast();
        int row = mapper->currentIndex();

        qDebug() << "New row: " << row;

        QSqlQuery query;
        QByteArray hashresult;
        QString shortHash;

        QCryptographicHash charUUID(QCryptographicHash::Sha1);

        int charID;
        QString label;
        QString state;

        // New characters are added to the end, so select that row in a
        // roundabout way...
        QSqlRecord last = charTable_p->record(row);

        qDebug() << last.value(QString("label")).toString();
        charID = last.value(QString("char_id")).toInt();
        charUUID.addData( last.value(QString("label")).toString().toLocal8Bit() );

        // Select and loop over the states as defined:
        query.exec(QString("SELECT label FROM states WHERE character = %1").arg(charID));
        while (query.next()) {
            charUUID.addData(query.value(0).toString().toLocal8Bit());
        }

        hashresult = charUUID.result();
        charUUID.reset();
        shortHash = QString(hashresult.toHex().remove(0, 2 * hashresult.size() - 7));

        qDebug() << "Shorthash: " << shortHash;

        query.prepare(QString("UPDATE characters SET char_GUUID = :id WHERE char_id = :char_id"));
        query.bindValue(":id", shortHash);
        query.bindValue(":char_id", charID);
        if(!query.exec()){
            qDebug() << query.lastError().text();
        }

        charTableView->setEnabled(true);
        newChar->setEnabled(true);
        deleteChar->setEnabled(true);

        charTable_p->select();
        statesTable_p->select();
    }
}

void CharacterEditorWindow::onCharacterClicked(const QModelIndex &index)
{
    mapper->setCurrentModelIndex(index);

    QString charID;
    charID = charTable_p->record(index.row()).value(QString("char_id")).toString();
    statesTable_p->filter().clear();
    statesTable_p->setFilter(QString("character = '%1'").arg(charID));
}


