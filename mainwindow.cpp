#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QListView>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QProgressDialog>
#include <QSqlRelationalDelegate>
#include <QScreen>
#include <QSizePolicy>
#include <QSplitter>
#include <QtSql>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWindow>

#include <iostream>

#include "mainwindow.h"
#include "nexusreader.h"
#include "mainmenu.h"
#include "charactereditorwindow.h"
#include "stateselectordelegate.h"
#include "noteditabledelegate.h"
#include "mdatabasemanager.h"
#include "nexuswriter.h"
#include "taxaeditorwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Initialize variables.
    taxonFilter = "";

    dbManager = new MDatabaseManager;

    // Initialise main window dimensions so it doesn't look stupid when it first opens up
    QRect rec;
    rec = QGuiApplication::primaryScreen()->geometry();
    int width = rec.width(); // / 4 * 3 ;
    int height = rec.height(); // / 4 * 3;
    resize(width, height);

    // Set up the interface
    createMenus();

    // Do the basic interface display setup with table views.
    QWidget *mwCentralWidget = new QWidget(this);
    setCentralWidget(mwCentralWidget);

    QGridLayout *mainLayout = new QGridLayout(mwCentralWidget);
    mwCentralWidget->setLayout(mainLayout);

    // Add the relevant table view widgets

    // Taxon table views
    taxaTableView = new QTableView(this);
    taxaTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    taxaTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    taxaTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QToolBar *taxaTools = new QToolBar(tr("Taxa"), mwCentralWidget);
    taxaTools->setWindowTitle(tr("Taxa"));
    QToolBar *obsTools = new QToolBar(tr("Observations"), mwCentralWidget);


    // Observation table views
    obsTableView = new QTableView(this);
    obsTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    obsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);


    // Set up the main tool bar

    // Taxa tools
    QPushButton *addTaxon = new QPushButton(tr("New taxon"), this);
    QPushButton *deleteTaxon = new QPushButton(tr("Delete taxon"), this);
    connect(addTaxon, &QPushButton::released, this, &MainWindow::getNewTaxon);

    QWidget *taxaToolsSpacer = new QWidget(taxaTools);
    taxaToolsSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    taxaTools->addWidget(addTaxon);
    taxaTools->addWidget(deleteTaxon);
    taxaTools->addWidget(taxaToolsSpacer);

    // Observation tools
    obsFilterField = new QLineEdit(this);
    QLabel *obsFilterLabel = new QLabel(this);
    obsFilterLabel->setBuddy(obsFilterField);
    obsFilterLabel->setText(tr("Filter observations:"));

    QPushButton *addObs = new QPushButton(tr("New observation"), this);
    addObs->setToolTip(tr("Use this to add additional observations for a character and taxon (e.g. to create a polymorphism)"));
    connect(addObs, &QPushButton::released, this, &MainWindow::insertObservation);

    QPushButton *clearObsFilterBtn = new QPushButton(tr("Clear"), this);
    clearObsFilterBtn->setToolTip(tr("Clear all active filters"));
    connect(clearObsFilterBtn, &QPushButton::released, this, &MainWindow::clearFilters);

    QWidget *obsToolsSpacer = new QWidget(obsTools);
    obsToolsSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    obsTools->addWidget(obsToolsSpacer);
    obsTools->addWidget(addObs);
    obsTools->addSeparator();
    obsTools->addSeparator();
    obsTools->addWidget(obsFilterLabel);
    obsTools->addWidget(obsFilterField);
    obsTools->addWidget(clearObsFilterBtn);

    mainLayout->addWidget(taxaTools, 1, 0);
    mainLayout->addWidget(obsTools, 1, 1, 1, -1);

    // Put the main tables in a splitter:
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter, 2, 0, 1, -1);
    splitter->addWidget(taxaTableView);
    splitter->addWidget(obsTableView);
    splitter->setStretchFactor(1, 2);
    mainLayout->setRowStretch(2, 3);

    // Add a console at the bottom of the screen to display some internal messages to the user
    // NOTE: this might get removed in later versions or be displayed in a separate window as in some other programs.
    console = new QPlainTextEdit(this);
    console->setFont(QFont("courier"));
    console->setReadOnly(true);
    console->resize(console->width(), 28);
    mainLayout->addWidget(console, 3, 0, 1, -1);

    writeToConsole("**** Welcome to INANE ****");
    writeToConsole(QString("Version %1").arg(VERSION_STRING));
}

void MainWindow::createMenus()
{
    mainMenu = new MainMenu(this);
    setMenuBar(mainMenu);
    mainMenu->setDataMenusEnabled(false);
}

void MainWindow::aboutMenu()
{
    QString about = QString(
    "INANE is Not a Nexus Editor\n\n"
    "version %1\n"
    "written by Martin D. Brazeau."
    "\n\n"
    "This program is written using the Qt Framework and is distributed as free, open source software.\n"
    "Despite its name, this program can still read and write Nexus files. Nexus importing uses the Nexus "
    "Class Library by Mark Holder and Paul O. Lewis.\n\n").arg(VERSION_STRING)
    ;

    showMessage(about);
}

void MainWindow::addTaxon(const QString &name)
{
    QSqlQuery query;
    QCryptographicHash charUUID(QCryptographicHash::Sha1);
    QByteArray data;
    QByteArray hashresult;

    data = name.toLocal8Bit();
    charUUID.addData(data);
    hashresult = charUUID.result();
    charUUID.reset();
    QString shortHash = QString(hashresult.toHex().remove(0, 2 * hashresult.size() - 7));

    query.prepare(QString("INSERT INTO taxa (taxon_GUUID, name, otu, taxgroup) VALUES (:taxon_id, :name, :name, (SELECT group_id FROM taxongroups WHERE groupname = 'default group'))"));
    query.bindValue(":taxon_id", shortHash);
    query.bindValue(":name", name);
    query.exec();
    if (query.next()) {
        qDebug() << query.value(0).toString();
    }
}

void MainWindow::writeToConsole(const QString &msg)
{
    console->moveCursor(QTextCursor::End);
    console->insertPlainText(msg + "\n");
    console->moveCursor(QTextCursor::End);
}

void MainWindow::showMessage(QString message)
{
    QMessageBox mb(this);
    mb.setText(message);

    mb.setStandardButtons(QMessageBox::Ok);
    mb.exec();
}

void MainWindow::dbNew()
{
    // TODO: put in a check and a dialog for when a database is already open
    // Alternatively: deactivate ("ghost") File > Open... when a file is already open

    QFileDialog fopen;

    path = fopen.getSaveFileName();
    if (path.isEmpty()) {
        return;
    }

    // Initiate database
    dbManager->openDatabase(path);

    createMainTables();
    configMainTables();
}

void MainWindow::dbOpen()
{
    QFileDialog fopen;

    path = fopen.getOpenFileName();

    if (path.isEmpty()) {
        return;
    }

    // Initiate database
    dbManager->openDatabase(path);

    configMainTables();
}

void MainWindow::dbClose()
{
    QString connection = QSqlDatabase::database().databaseName();
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase(connection);

    path = "";

    resetMainTables();

    mainMenu->setInFileMenusEnabled(true);
    mainMenu->setDataMenusEnabled(false);
}

void MainWindow::dbSave()
{

}

void MainWindow::importNexus()
{
//    if (QSqlDatabase::database().databaseName() == "") {
//        QMessageBox::critical(nullptr, "No database exists", "Create a new empty database before attempting to import files.");
//        return;
//    }

    QFileDialog fopen;
    QString nexusfilename;
    NexusReader nxreader;

    nexusfilename = fopen.getOpenFileName(nullptr, QString("Choose a Nexus file"));

    if (nexusfilename.isEmpty()) {
        return;
    }

    dbNew();
    if (path.isEmpty()) {
        return;
    }

    QByteArray nxfilename = nexusfilename.toLocal8Bit();
    char* cfilename = nxfilename.data();

    try {
        nxreader.openNexusFile(cfilename);
    }  catch (NxsException &e) {
        showMessage(QString::fromStdString(e.what()));
        return;
    }

    QSqlQuery query;
    QCryptographicHash charUUID(QCryptographicHash::Sha1);
    QByteArray data;
    QByteArray hashresult;

    unsigned int i, j, k;

    // Process the taxa and insert them to the taxa table
    QProgressDialog progress("Importing taxa...", "Abort", 0, nxreader.getNtax(), this);
    progress.setWindowModality(Qt::WindowModal);
    for (i = 0; i < nxreader.getNtax(); ++i) {
        progress.setValue(i);
        QString taxname = nxreader.getTaxLabel(i);
        addTaxon(taxname);
    }
    progress.setValue(nxreader.getNtax());

    // Process the characters and insert them into the characters table
    progress.reset();
    progress.setLabelText("Importing characters...");
    progress.setMinimum(0);
    progress.setMaximum(nxreader.getNchar());
    for (i = 0; i < nxreader.getNchar(); ++i) {
        progress.setValue(i);
        QString label = nxreader.getCharLabel(i);

        if (label == " ") {
            writeToConsole(QString("Warning: character %1 of import file has no character label defined.").arg(i + 1));
            label = QString("Character %1").arg(i+1);
        }

        charUUID.addData(label.toLocal8Bit());

        for (j = 0; j < nxreader.getNumStatesForChar(i); ++j) {
            QString label = nxreader.getStateLabel(i, j);
            if (label != " ") {
                charUUID.addData(label.toLocal8Bit());
            }
        }

        // TODO: Remove automatic GUUIDing.
        hashresult = charUUID.result();
        charUUID.reset();
        QString shortHash = QString(hashresult.toHex().remove(0, 2 * hashresult.size() - 7));
        query.prepare(QString("INSERT INTO characters (char_GUUID, charlabel) VALUES (:char_id, :label)"));
        query.bindValue(":char_id", shortHash);
        query.bindValue(":label", label);
        if (!query.exec()) {
            qDebug() << "Error inserting " << label;
            writeToConsole("Error inserting " + label);
        }

        if (!query.exec(QString("SELECT last_insert_rowid() FROM characters"))) {
            qDebug() << "Error selecting " << shortHash;
        }

        query.next();
        int charID = query.value(0).toInt();

        // Insert into the state table
        // TODO: Also need to get the OBSERVED number of states and check for mismatches.
        for (j = 0; j < nxreader.getNumStatesForChar(i); ++j) {
            QString stateLabel = QString(nxreader.getStateLabel(i, j).toLocal8Bit());
            if (stateLabel != " ") {
                query.prepare(QString("INSERT INTO states (character, statelabel, symbol) VALUES (:character, :label, "
                                      "(SELECT symbol_id FROM symbols WHERE symbol_id = :symbol_id)"
                                      ")"));
                query.bindValue(":character", charID);
                query.bindValue(":label", stateLabel);
                query.bindValue(":symbol_id", j + 1);
                if (!query.exec()) {
                    qDebug() << query.lastError().text();
                    writeToConsole(query.lastError().text());
                }
            }
        }
    }

    progress.setValue(nxreader.getNtax());

    progress.reset();
    progress.setLabelText("Importing matrix data...");
    progress.setMinimum(0);
    progress.setMaximum(nxreader.getNtax() * nxreader.getNchar());

    // Process the individual observations in the matrix and insert them to the observations table
    int c = 0;
    for (i = 0; i < nxreader.getNtax(); ++i) {
//        progress.setValue(i);
        QString taxname = nxreader.getTaxLabel(i);

        int taxID;
        int charID;

        query.exec(QString("SELECT taxon_id FROM taxa WHERE name = '%1'").arg(taxname));
        while (query.next()) {
            taxID = query.value(0).toInt();
            qDebug() << taxID << " " << taxname;
        }

        for (j = 0; j < nxreader.getNchar(); ++j) {
            progress.setValue(c);
            c++;
            QString charlabel = nxreader.getCharLabel(j);
            // TODO: Check this line: it works only because nothing else is happing during import. Beware indices!
            query.exec(QString("SELECT char_id FROM characters WHERE rowid = %1").arg(j + 1));
            while (query.next()) {
                charID = query.value(0).toInt();
                qDebug() << "Char ID: " << charID;
            }

//            qDebug() << "Character: " << charlabel;
            QSqlDatabase::database().transaction();
            for (k = 0; k < nxreader.getNumStates(i, j); ++k) {
                QString statelabel;
                statelabel = nxreader.getStateLabel(i, j, k);
//                qDebug() << "state: " << statelabel;
                if ((statelabel != QString("missing")) && (statelabel != QString("inapplicable"))) {
                    query.prepare(QString("INSERT INTO observations (taxon, character, state) "
                                          "VALUES (:taxon, :character, (SELECT state_id FROM states WHERE character = :character AND statelabel = :label))"));
                    query.bindValue(":taxon", taxID);
                    query.bindValue(":character", charID);
                    query.bindValue(":label", statelabel);
                    if (!query.exec()) {
                        qDebug() << query.lastError().text();
                        writeToConsole(query.lastError().text());
                    }
                } else {
                    query.prepare(QString("INSERT INTO observations (taxon, character, state) "
                                          "VALUES (:taxon, :character, (SELECT state_id FROM states WHERE statelabel = :label))"));
                    query.bindValue(":taxon", taxID);
                    query.bindValue(":character", charID);
                    query.bindValue(":label", statelabel);
                    if (!query.exec()) {
                        qDebug() << query.lastError().text();
                        writeToConsole(query.lastError().text());
                    }
                }
            }
            QSqlDatabase::database().commit();
        }
    }

    progress.setMaximum(nxreader.getNchar());

    taxaTable->select();
    charTable->select();
    observationsTable->select();

    onDataChanged();
    onTaxaChanged();

    nxreader.closeNexusFile();
}

void MainWindow::exportNexus()
{
    std::ofstream nexout;

    QFileDialog fopen;

    nexout.open(fopen.getSaveFileName().toStdString());

    if (!nexout.is_open()) {
        return; // TODO: Put in an error?
    }

    NexusWriter::write(nexout);

//    nexout << nexustext.toStdString();

    nexout.close();
}

void MainWindow::openTaxaTableView()
{
    TaxaEditorWindow *taxaEditor = new TaxaEditorWindow; //(this);
    taxaEditor->show();
}

void MainWindow::onDataChanged()
{
    observationsTable->select();
    obsTableView->resizeColumnsToContents();
}

void MainWindow::onTaxaChanged()
{
    taxaTable->select();
    taxaTableView->resizeColumnsToContents();
}

void MainWindow::onTaxonSelected(const QModelIndex &index)
{
    QSqlRecord record = taxaTable->record(index.row());
    QVariant taxonID = record.value(QString("taxon_id"));

//    qDebug() << "Selected: " << taxonID.toString();

    taxonFilter = taxonID.toString();

    obsFilterField->clear();

    observationsTable->setFilter(QString("taxon = '%1'").arg(taxonID.toString()));
    observationsTable->select();
}

void MainWindow::onObsFilterEdited(const QString &string)
{
    // This conditional clears the filter if no string is input (i.e. when a filter is erased)
    if (string == "") {
        observationsTable->setFilter("");
        observationsTable->select();
        return;
    }

    QSqlQuery query;
    query.prepare(QString("SELECT char_id FROM characters WHERE charlabel LIKE :label "));
    query.bindValue(":label", QString("%%1%").arg(string));
    query.exec();
    query.next();

    QString filter = "(observations.character = ";
    filter += (QString::number(query.value(0).toInt()) + QString(" "));
    while (query.next()) {
        filter += QString(" OR observations.character = ");
        filter += (QString::number(query.value(0).toInt()) + QString(" "));
    }
    filter += QString(")");

    // TODO: Fix the taxon filter as it should no longer rely on a string
    if (taxonFilter != "") {
        filter += (QString(" AND (observations.taxon = '%1')").arg(taxonFilter));
    }

//    qDebug() << filter;

    observationsTable->setFilter(filter);
    observationsTable->select();
}

void MainWindow::openCharTableView()
{
    /* This block helps trigger the character editor at the character corresponding to the
     * the current character of the slected row of the observations table. */
    // TODO: it will be good to review this logic.
    int obsID;
    obsID = observationsTable->record(obsTableView->currentIndex().row()).field("id").value().toInt();
    qDebug() << "ID of selected observation: " << obsID;
    QSqlQuery query;
    query.exec(QString("SELECT character FROM observations WHERE id = %1").arg(obsID));
    query.next();
    int charID = query.value(0).toInt();


    CharacterEditorWindow *charEditor = new CharacterEditorWindow(charID);
    connect(charTable, &QSqlRelationalTableModel::dataChanged, this, &MainWindow::onDataChanged);
    connect(stateTable, &QSqlRelationalTableModel::dataChanged, this, &MainWindow::onDataChanged);
    connect(charEditor, &CharacterEditorWindow::destroyed, this, &MainWindow::updateObsTable);
    charEditor->setAttribute(Qt::WA_DeleteOnClose);
    charEditor->show();
}

void MainWindow::openStateTableView()
{
    statesWindow = new QWidget;

    stateTableView = new QTableView(statesWindow);
    stateTableView->setModel(stateTable);
//    stateTable->setTable("states");
    stateTable->select();
//    symbolsTable->select();
    stateTable->setFilter("");

    statesWindow->setLayout(new QHBoxLayout(stateTableView));
    statesWindow->layout()->addWidget(stateTableView);

//    stateTableView->resizeColumnsToContents();

//    connect(stateTable, &QSqlRelationalTableModel::dataChanged, this, &MainWindow::onDataChanged);

//    statesWindow->setMinimumWidth(stateTableView->horizontalHeader()->length());


    stateTableView->show();
    statesWindow->show();
}

void MainWindow::showInitDialog()
{
    QDialog initDialog;
    initDialog.setWindowTitle("Welcome to INANE");
    QVBoxLayout *layout = new QVBoxLayout(&initDialog);

    QLabel infoText("Create a new database or open an existing one.", &initDialog);

    QPushButton *newButton  = new QPushButton("New", &initDialog);
    QPushButton *openButton = new QPushButton("Open", &initDialog);
    QPushButton *quitButton = new QPushButton("Quit", &initDialog);

    layout->addWidget(&infoText);
    layout->addWidget(newButton);
    layout->addWidget(openButton);
    layout->addWidget(quitButton);

    initDialog.setLayout(layout);

    connect(newButton, &QPushButton::clicked, this, [&](){
        initDialog.close();
        dbNew();
    });

    connect(openButton, &QPushButton::clicked, this, [&](){
        initDialog.close();
        dbOpen();
    });

    connect(quitButton, &QPushButton::clicked, this, [&](){
        initDialog.close();
    });

    initDialog.exec();
}

void MainWindow::createMainTables()
{
    MDatabaseManager::createMainTables();
}

void MainWindow::configMainTables()
{
    taxaTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    taxaTable->setTable("taxa");
    taxaTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);
    taxaTable->setHeaderData(2, Qt::Horizontal, tr("Taxon"));

    groupsTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    groupsTable->setTable("taxongroups");

    taxaTable->setRelation(4, QSqlRelation("taxongroups", "group_id", "groupname"));

    charTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    charTable->setTable("characters");
    charTable->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);

    stateTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    stateTable->setTable("states");
    stateTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);

    symbolsTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    symbolsTable->setTable("symbols");
    stateTable->setRelation(1, QSqlRelation("symbols", "symbol_id", "symbol"));
    stateTable->setRelation(2, QSqlRelation("characters", "char_id", "charlabel"));

    observationsTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    observationsTable->setTable("observations");
    observationsTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);

    observationsTable->setRelation(1, QSqlRelation("taxa", "taxon_id", "name"));
    observationsTable->setRelation(2, QSqlRelation("characters", "char_id", "charlabel"));
    observationsTable->setRelation(3, QSqlRelation("states", "state_id", "statelabel"));

    observationsTable->setHeaderData(0, Qt::Horizontal, tr("ID"));
    observationsTable->setHeaderData(1, Qt::Horizontal, tr("Taxon"));
    observationsTable->setHeaderData(2, Qt::Horizontal, tr("Character"));
    observationsTable->setHeaderData(3, Qt::Horizontal, tr("State"));
    observationsTable->setHeaderData(4, Qt::Horizontal, tr("Notes"));

    StateSelectorDelegate *obsDelegate = new StateSelectorDelegate(observationsTable);
    obsTableView->setItemDelegateForColumn(3, obsDelegate);
    obsTableView->setItemDelegateForColumn(1, new NotEditableDelegate(obsTableView));
    obsTableView->setItemDelegateForColumn(2, new NotEditableDelegate(obsTableView));
    obsTableView->setAlternatingRowColors(true);

    taxaTableView->setModel(taxaTable);
    obsTableView->setModel(observationsTable);

    taxaTable->select();
    charTable->select();
    symbolsTable->select();
    stateTable->select();
    observationsTable->select();

    // Display the tables
    taxaTableView->setColumnHidden(0, true);
    taxaTableView->setColumnHidden(1, true);
    taxaTableView->setColumnHidden(3, true);
    taxaTableView->setColumnHidden(4, true);
    taxaTableView->setColumnHidden(5, true);
    taxaTableView->setColumnHidden(6, true);
    taxaTableView->horizontalHeader()->setStretchLastSection(true);

    obsTableView->setColumnHidden(0, true);
    obsTableView->setSortingEnabled(true);

    taxaTableView->resizeColumnsToContents();
    obsTableView->resizeColumnsToContents();
    obsTableView->horizontalHeader()->setStretchLastSection(true);

    connect(taxaTableView, &QAbstractItemView::clicked, this, &MainWindow::onTaxonSelected);
    connect(obsFilterField, &QLineEdit::textEdited, this, &MainWindow::onObsFilterEdited);
//    connect(observationsTable, &QSqlRelationalTableModel::dataChanged, obsTableView, &QTableView::resizeColumnsToContents);
//    connect(obsDelegate, &StateSelectorDelegate::commitData, observationsTable, &QSqlRelationalTableModel::select);

    mainMenu->setDataMenusEnabled(true);
    mainMenu->setInFileMenusEnabled(false);
    //    taxaTableView->resize(20, );
}

void MainWindow::resetMainTables()
{
    delete taxaTable;
    delete charTable;
    delete stateTable;
    delete observationsTable;
}

void MainWindow::getNewTaxon()
{
    bool ok;

    if (!QSqlDatabase::database().defaultConnection) {
        writeToConsole("No database open");
        return;
    }

    QString taxonName = QInputDialog::getText(nullptr,
                                              tr("New taxon"),
                                              tr("Input a new taxon name"),
                                              QLineEdit::Normal,
                                              "",
                                              &ok);

    if (ok && !taxonName.isEmpty()) {
        addTaxon(taxonName);
    } else {
        return;
    }

    taxaTable->select();
}

void MainWindow::updateObsTable()
{
    // TODO: Check for valid taxa and characters?
    // TODO: We have two very similar functions!
    QSqlQuery query;

    observationsTable->select();
    obsTableView->resizeColumnsToContents();
}

void MainWindow::insertObservation()
{
    int taxID = -1, charID = -1, stateID = -1;

    QModelIndex index = obsTableView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    int row = sqlModel->record(index.row()).value("id").toInt();

    QSqlQueryModel qmodel;

    qmodel.setQuery(QString("SELECT taxon, character FROM observations WHERE rowid = %1").arg(row));
    taxID = qmodel.record(0).value("taxon").toInt();
    charID = qmodel.record(0).value("character").toInt();

    qmodel.setQuery("SELECT state_id FROM states WHERE statelabel = 'missing'");
    stateID = qmodel.record(0).value("state_id").toInt();

    MDatabaseManager::addObservation(taxID, charID, stateID);
    observationsTable->select();
}

void MainWindow::clearFilters()
{
    observationsTable->setFilter("");
    taxonFilter = "";
    obsFilterField->clear();
}


