#include <QApplication>
#include <QDialogButtonBox>
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
#include <QPixmap>
#include <QPushButton>
#include <QtSql>
#include <QSqlError>
#include <QStyle>
#include <QTableView>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "charactereditorwindow.h"
#include "mainwindow.h"
#include "noteditabledelegate.h"

CharacterEditorWindow::CharacterEditorWindow(const int row, QWidget *parent) : QWidget(parent)
{
    setWindowModality(Qt::ApplicationModal);
    charTableView = new QTableView(this);
    charWindLayout = new QGridLayout(this);
    charWindLayout->addWidget(charTableView, 0, 0, 8, 1);

    charListButtons = new QDialogButtonBox(this);
    charListButtons->setOrientation(Qt::Horizontal);
    newChar = charListButtons->addButton(tr("New character"), QDialogButtonBox::ActionRole);
    deleteChar = charListButtons->addButton(tr("Delete character"), QDialogButtonBox::ActionRole);
    connect(newChar, &QPushButton::released, this, &CharacterEditorWindow::newCharacterAction);
    connect(deleteChar, &QPushButton::released, this, &CharacterEditorWindow::deleteCharAction);

    charWindLayout->addWidget(charListButtons, 8, 0);

    charTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    charTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    charTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    charTable_p = nullptr;

    setCharTable();

    initEditorArea(row);

    charTableView->resizeColumnsToContents();
    charTableView->horizontalHeader()->setStretchLastSection(true);
}

void CharacterEditorWindow::setCharTable()
{
//    charTable_p = charTable;
    charTable_p = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    charTable_p->setTable("characters");
    charTableView->setModel(charTable_p);
    charTable_p->select();

    charTableView->setColumnHidden(0, true);
    charTableView->setColumnHidden(1, true);
    charTableView->setColumnHidden(3, true);
    charTableView->setColumnHidden(4, true);
    charTableView->setColumnHidden(charTable_p->fieldIndex("notes"), true);

    charTableView->selectRow(0);

    statesTable_p = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    statesTable_p->setTable("states");
    statesTable_p->setRelation(1, QSqlRelation("symbols", "symbol_id", "symbol"));
    statesTable_p->setRelation(2, QSqlRelation("characters", "char_id", "charlabel"));
    statesTable_p->select();
}

void CharacterEditorWindow::initEditorArea(const int row)
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
    mapper->addMapping(labelField, charTable_p->fieldIndex("charlabel"));
    mapper->addMapping(sourceField, charTable_p->fieldIndex("source"));
    mapper->addMapping(descripField, charTable_p->fieldIndex("notes"));

    // Set up the states table:
    statesTable->setModel(statesTable_p);
    statesTable->setColumnHidden(0, true);
    statesTable->setColumnHidden(2, true);
    statesTable->horizontalHeader()->setStretchLastSection(true);
    statesTable->resizeColumnsToContents();
    statesTable->setItemDelegateForColumn(statesTable_p->fieldIndex("symbol"), new NotEditableDelegate(this));
    statesTable_p->select();

    // TODO: Clean this crap.
    if (row > 0) {
        mapper->setCurrentIndex(row-1);
        charTableView->selectRow(row-1);
        onCharacterClicked(charTableView->currentIndex());
    } else {
        mapper->toFirst();
        int charID;
        charID = charTable_p->record(0).value(QString("char_id")).toInt();
        statesTable_p->setFilter(QString("states.character = %1").arg(charID));
    }
    connect(charTableView, &QTableView::clicked, this, &CharacterEditorWindow::onCharacterClicked);

    // Place in layout
    charWindLayout->addWidget(labelLabel, 0, 1);
    charWindLayout->addWidget(labelField, 1, 1, 1, -1);
    charWindLayout->addWidget(statesLabel, 2, 1);
    charWindLayout->addWidget(statesTable, 3, 1);
    charWindLayout->addWidget(sourceLabel, 4, 1);
    charWindLayout->addWidget(sourceField, 5, 1, 1, -1);
    charWindLayout->addWidget(descripLabel, 6, 1);
    charWindLayout->addWidget(descripField, 7, 1, 1, -1);

    submitButton = new QPushButton(tr("Submit"), this);
    charWindLayout->addWidget(submitButton, 8, 1);
    connect(submitButton, &QPushButton::released, this, &CharacterEditorWindow::commitCharChange);

    // Add some up-down buttons for reordering states
//    stateTools = new QToolBar(this);

    stateTools = new QToolBar(this);
    stateTools->setOrientation(Qt::Vertical);
    moveStateUp = new QToolButton(this);
    moveStateUp->setArrowType(Qt::UpArrow);
    moveStateUp->setToolTip(tr("Reorder state up"));
    moveStateDn = new QToolButton(this);
    moveStateDn->setArrowType(Qt::DownArrow);
    moveStateDn->setToolTip(tr("Reorder state down"));
    newState = new QToolButton(this);
    newState->setIcon(QIcon(":/resources/icons/add_new.png"));
    newState->setToolTip(tr("Add a state to this character"));
    deleteState = new QToolButton(this);
    deleteState->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton));
    deleteState->setToolTip(tr("Delete state from this character"));
    stateTools->addWidget(moveStateUp);
    stateTools->addWidget(moveStateDn);
    stateTools->addWidget(newState);
    stateTools->addWidget(deleteState);
    charWindLayout->addWidget(stateTools, 3, 2);

    // Create state button connections
    connect(newState, &QToolButton::released, this, &CharacterEditorWindow::newStateAction);
    connect(deleteState, &QToolButton::released, this, &CharacterEditorWindow::deleteStateAction);
}

void CharacterEditorWindow::deleteCharAction()
{
    QMessageBox checkDelete;
    checkDelete.setIcon(QMessageBox::Critical);
    checkDelete.setText("Are you sure you want to delete this character?");
    checkDelete.setInformativeText("Deleting characters is not best practice. If you want to exclude a character from analyses, you should toggle its inclusion status.");
    checkDelete.setStandardButtons(QMessageBox::Abort | QMessageBox::Ignore);
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

    statesTable_p->setFilter(""); // Temporarily disable filter

    QSqlQuery query;
    if(!query.exec("INSERT INTO characters (charlabel) VALUES ('new character')")) {
        qDebug() << "Failed to create new character!";
    }

    charTable_p->select();
    charTableView->resizeColumnsToContents();

    mapper->toLast();
    qDebug() << "Post-insert row: " << mapper->currentIndex();

    int newID;
    query.exec("SELECT last_insert_rowid()");
    query.next();
    newID = query.value(0).toInt();
    query.exec(QString("INSERT INTO states (symbol, character, statelabel) VALUES (1, %1, 'state_1')").arg(newID));
    query.exec(QString("INSERT INTO states (symbol, character, statelabel) VALUES (2, %1, 'state_2')").arg(newID));

    statesTable_p->setFilter(QString("character = %1").arg(newID)); //Re-enable filter on new characters

    charTableView->setEnabled(false);
    newChar->setEnabled(false);
    deleteChar->setEnabled(false);

    // Put the new character into the observations tabe
    if(!query.exec("INSERT INTO observations (taxon, character, state) "
                    "SELECT taxa.taxon_id, char_id, states.state_id "
                    "FROM taxa CROSS JOIN (SELECT * FROM characters WHERE char_id NOT IN (SELECT character FROM observations)) "
                    "CROSS JOIN states WHERE states.statelabel = 'missing'")){
        qDebug() << query.lastError().text();
        return;
    } else {
        qDebug() << "Query executed";
    }

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

        qDebug() << last.value(QString("charlabel")).toString();
        charID = last.value(QString("char_id")).toInt();
        charUUID.addData( last.value(QString("charlabel")).toString().toLocal8Bit() );

        QSqlDatabase::database().transaction();
        // Select and loop over the states as defined:
        query.exec(QString("SELECT statelabel FROM states WHERE character = %1").arg(charID));
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

        if (!charTable_p->submitAll()) {
            QSqlDatabase::database().rollback();
        } else {
            QSqlDatabase::database().commit();
        }

        charTableView->setEnabled(true);
        newChar->setEnabled(true);
        deleteChar->setEnabled(true);

        statesTable_p->submitAll();

        charTable_p->select();
        statesTable_p->select();
    }
}

void CharacterEditorWindow::newStateAction()
{
    // TODO: This stuff could be made a generic utility
    QModelIndex index;
    index = charTableView->currentIndex();
    QSqlQueryModel *m = qobject_cast<QSqlQueryModel *>(charTableView->model());
    QSqlRecord rec = m->record(index.row());
    int rowid = rec.field("characters.char_id").value().toInt();
    // END TODO.

    QSqlQuery query;
    int charID;
    if (!query.exec(QString("SELECT char_id FROM characters WHERE rowid = %1").arg(rowid))) {
        qDebug() << "Cannot select char id: " << charID << ". " << query.lastError().text();
    }
    query.next();
    charID = query.value(0).toInt();

    int symbolID;
    int symbolRank;
    // Get a symbol not already in use; reject the request and throw an error if state limit exceeded.
    if(!query.exec(QString("SELECT symbol_id, rank FROM symbols "
                            "WHERE symbol_id NOT IN (SELECT symbol FROM states WHERE character = %1) "
                            " AND symbol != '?' AND symbol != '-' "
                            " ORDER BY symbol_id ASC").arg(charID))) {

        QMessageBox newStateFail;
        newStateFail.setIcon(QMessageBox::Critical);
        newStateFail.setText("Cannot create new state");
        newStateFail.setInformativeText("This is likely because you have reached the limit of states for this character");
        newStateFail.exec();
        return;
    }

    query.next();
    symbolID = query.value(0).toInt();
    symbolRank = query.value(1).toInt();

    query.prepare(QString("INSERT INTO states (symbol, statelabel, character) VALUES (:symbol, :label, :char_id)"));
    query.bindValue(":symbol", symbolID);
    query.bindValue(":label", QString("new_state_%1").arg(symbolRank));
    query.bindValue(":char_id", charID);
    if (!query.exec()) {
        qDebug() << query.lastError().text();
        QMessageBox newStateFail;
        newStateFail.setIcon(QMessageBox::Critical);
        newStateFail.setText("Cannot create new state");
        newStateFail.setInformativeText("This is likely because you have reached the limit of states for this character");
        newStateFail.exec();
    }
    statesTable_p->select();
    filterStatesByChar(index);
}

void CharacterEditorWindow::deleteStateAction()
{
    // TODO: This stuff could be made a generic utility
    QModelIndex index;
    index = charTableView->currentIndex();
    QSqlQueryModel *m = qobject_cast<QSqlQueryModel *>(charTableView->model());
    QSqlRecord rec = m->record(index.row());
    int charID = rec.field("char_id").value().toInt();
    // END TODO.

    // TODO: This stuff could be made a generic utility
    // As above, get the state row id now
    index = statesTable->currentIndex();
    m = qobject_cast<QSqlQueryModel *>(statesTable->model());
    rec = m->record(index.row());
    int stateID = rec.field("state_id").value().toInt();

    QSqlQuery query;

    // First, replace all instances of this character in the observations table with missing
    query.prepare("UPDATE observations "
                  "SET state = (SELECT state_id FROM states WHERE statelabel = 'missing' ) "
                  "WHERE state = :state_id");
    query.bindValue(":char_id", charID);
    query.bindValue(":state_id", stateID);
    if (!query.exec()) {
        qDebug() << "\nUnable to to set states " << stateID << " to missing for char_id: " << charID;
        qDebug() << query.lastError().text() << "\n\n";
    }

    query.exec(QString("DELETE FROM states WHERE state_id = %1").arg(stateID));
    statesTable_p->select();
}

void CharacterEditorWindow::filterStatesByChar(const QModelIndex &index)
{
    int charID;
    charID = charTable_p->record(index.row()).value(QString("char_id")).toInt();
    statesTable_p->filter().clear();
    qDebug() << "CHAR ID: " << charID;
    statesTable_p->setFilter(QString("character = %1 ").arg(charID));
    statesTable_p->setSort(1, Qt::AscendingOrder);
    statesTable_p->select();
    statesTable->resizeColumnsToContents();
}

void CharacterEditorWindow::onCharacterClicked(const QModelIndex &index)
{
    mapper->setCurrentModelIndex(index);
    filterStatesByChar(index);
}


