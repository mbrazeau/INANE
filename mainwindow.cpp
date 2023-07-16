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
#include <QPushButton>
#include <QScreen>
#include <QSpacerItem>
#include <QSplitter>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QTableView>
#include <QVBoxLayout>
#include <QWindow>

#include <iostream>

#include "mainwindow.h"
#include "nexusreader.h"
#include "mainmenu.h"
#include "charactereditorwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Initialize variables.
    taxonFilter = "";

    // Initialise the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // Initialise main window dimensions so it doesn't look stupid when it first opens up
    QRect rec;
    rec = QGuiApplication::primaryScreen()->geometry();
    int width = rec.width() / 4 * 3 ;
    int height = rec.height() / 4 * 3;
    resize(width, height);

    // Set up the interface
    createMenus();

    // Do the basic interface display setup with table views.
    QWidget *myCentralWidget = new QWidget(this);
    setCentralWidget(myCentralWidget);

    QGridLayout *mainLayout = new QGridLayout(myCentralWidget);
    myCentralWidget->setLayout(mainLayout);

    // Add the relevant tables
    taxaTableView = new QTableView(this);
    taxaTableView->setModel(taxaTable);
    taxaTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    taxaTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    taxaTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    obsTableView = new QTableView(this);
    obsTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

//    QLineEdit *taxaFilterField = new QLineEdit(this);
//    QLabel *taxFilterLabel = new QLabel(this);
//    taxFilterLabel->setText(tr("Filter taxa:"));
    obsFilterField = new QLineEdit(this);
    QLabel *obsFilterLabel = new QLabel(this);
    obsFilterLabel->setBuddy(obsFilterField);
    obsFilterLabel->setText(tr("Filter observations:"));

//    mainLayout->addWidget(taxFilterLabel, 0, 0);
//    mainLayout->addWidget(taxaFilterField, 0, 1);
    mainLayout->addWidget(obsFilterLabel, 0, 2);
    mainLayout->addWidget(obsFilterField, 0, 3);
//    mainLayout->addWidget(taxaTableView, 1, 0);
//    mainLayout->addWidget(obsTableView, 1, 1, 1, -1);
//    mainLayout->setColumnStretch(1, 2);

    // Put the main tables in a splitter:
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter, 1, 0, 1, -1);
    splitter->addWidget(taxaTableView);
    splitter->addWidget(obsTableView);
    splitter->setStretchFactor(1, 2);

    QPushButton *addObs = new QPushButton(tr("New observation"), this);
    mainLayout->addWidget(addObs, 2, 3);
    mainLayout->setColumnStretch(1, 2);
}

void MainWindow::createMenus()
{
    mainMenu = new MainMenu(this);
    mainMenu->setDataMenusEnabled(false);
}

void MainWindow::aboutMenu()
{
    QString about =
    "INANE is Not a Nexus Editor\n\n"
    "version 0.1 alpha\n"
    "written by Martin D. Brazeau."
    "\n\n"
    "This program is written using the Qt Framework and is distributed as free, open source software.\n"
    "Nexus importing uses the Nexus Class Library by Mark Holder and Paul O. Lewis.\n\n"
    ;

    showMessage(about);
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
    QSqlDatabase::database().setDatabaseName(path);
    if (!QSqlDatabase::database().open()) {
        qDebug() << "No database!";
    }

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
    QSqlDatabase::database().setDatabaseName(path);
    if (!QSqlDatabase::database().open()) {
        qDebug() << "No database!";
    }

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
    for (i = 0; i < nxreader.getNtax(); ++i) {
        QString taxname = nxreader.getTaxLabel(i);
        data = taxname.toLocal8Bit();
        charUUID.addData(data);
        hashresult = charUUID.result();
        charUUID.reset();
        QString shortHash = QString(hashresult.toHex().remove(0, 2 * hashresult.size() - 7));
        query.prepare(QString("INSERT INTO taxa (taxon_GUUID, name) VALUES (:taxon_id, :name)"));
        query.bindValue(":taxon_id", shortHash);
        query.bindValue(":name", taxname);
        query.exec();
        if (query.next()) {
            qDebug() << query.value(0).toString();
        }
    }

    // Process the characters and insert them into the characters table
    unsigned int ctr = 1;
    for (i = 0; i < nxreader.getNchar(); ++i) {
        QString label = nxreader.getCharLabel(i);
        charUUID.addData(label.toLocal8Bit());

        qDebug() << label;

        for (j = 0; j < nxreader.getNumStatesForChar(i); ++j) {
            QString label = nxreader.getStateLabel(i, j);
            if (label != " ") {
                charUUID.addData(label.toLocal8Bit());
            }
        }

        hashresult = charUUID.result();
        charUUID.reset();
        QString shortHash = QString(hashresult.toHex().remove(0, 2 * hashresult.size() - 7));
        query.prepare(QString("INSERT INTO characters (char_GUUID, label) VALUES (:char_id, :label)"));
        query.bindValue(":char_id", shortHash);
        query.bindValue(":label", label);
        if (!query.exec()) {
            qDebug() << "Error inserting " << label;
        }

        if (!query.exec(QString("SELECT char_id FROM characters WHERE char_GUUID = '%1'").arg(shortHash))) {
            qDebug() << "Error selecting " << shortHash;
        }
        query.next();
        int charID = query.value(0).toInt();
        qDebug() << "Char ID: " << charID;
        // Insert into the state table
        for (j = 0; j < nxreader.getNumStatesForChar(i); ++j) {
            QString stateLabel = QString(nxreader.getStateLabel(i, j).toLocal8Bit());
            if (stateLabel != " ") {
                query.prepare(QString("INSERT INTO states (character, label) VALUES (:character, :label)"));
                //            query.bindValue(":state_id", ctr);
                query.bindValue(":character", charID);
                query.bindValue(":label", stateLabel);
                query.exec();
                ctr++;
            }
        }
        query.exec(QString("INSERT INTO states (symbol, character, label) VALUES ('?', %1, '%2')").arg(charID).arg(QString("missing")));
        query.exec(QString("INSERT INTO states (symbol, character, label) VALUES ('-', %1, '%2')").arg(charID).arg(QString("inapplicable")));
        ctr++;
    }

    // Process the individual observations in the matrix and insert them to the observations table
    ctr = 1;
    for (i = 0; i < nxreader.getNtax(); ++i) {
        QString taxname = nxreader.getTaxLabel(i);

        int taxID;
        int charID;

        query.exec(QString("SELECT taxon_id FROM taxa WHERE name = '%1'").arg(taxname));
        while (query.next()) {
            taxID = query.value(0).toInt();
            qDebug() << taxID << " " << taxname;
        }

        for (j = 0; j < nxreader.getNchar(); ++j) {
            QString charlabel = nxreader.getCharLabel(j);
            query.exec(QString("SELECT char_id FROM characters WHERE rowid = %1").arg(j + 1));
            while (query.next()) {
                charID = query.value(0).toInt();
//                qDebug() << "Char ID: " << charID;
            }

//            qDebug() << "Character: " << charlabel;
            QSqlDatabase::database().transaction();
            for (k = 0; k < nxreader.getNumStates(i, j); ++k) {
                QString statelabel;
                statelabel = nxreader.getStateLabel(i, j, k);
//                qDebug() << "state: " << statelabel;
                query.prepare(QString("INSERT INTO observations (taxon, character, state) "
                                      "VALUES (:taxon, :character, (SELECT state_id FROM states WHERE character = :character AND label = :label))"));
//                query.bindValue(":obs_id", ctr);
                query.bindValue(":taxon", taxID);
                query.bindValue(":character", charID);
                query.bindValue(":label", statelabel);
                query.exec();
//                qDebug() << query.lastError().text();
                ++ctr;
            }
            QSqlDatabase::database().commit();
        }
    }

    // Populate the states table by inserting the distinct set of states and character observations
//    query.exec("INSERT INTO states (character, label) SELECT DISTINCT character, state FROM observations");

    taxaTable->select();
    charTable->select();
    observationsTable->select();

    onDataChanged();
    onTaxaChanged();

    // Close the file, it's no longer needed.
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

    nexout << QString("#NEXUS\n\n").toStdString();

    QSqlQuery query;




    nexout << "BEGIN TAXA;\n";

    query.exec(QString("SELECT COUNT(*) FROM taxa"));
    query.next();
    int ntax = query.value(0).toInt();
    nexout << QString("DIMENSIONS NTAX = %1;\nTAXLABELS\n").arg(ntax).toStdString();

    query.exec(QString("SELECT name FROM taxa"));
    while (query.next()) {
        QString label;
        label = query.value(0).toString();
        nexout << NxsString::GetEscaped(label.toStdString());
        nexout << "\n";
    }

    nexout << ";\nEND;\n\n";

    nexout << "BEGIN CHARACTERS;\n";
    query.exec(QString("SELECT COUNT(*) FROM characters"));
    query.next();
    int nchar = query.value(0).toInt();
    query.exec(QString("SELECT label FROM characters"));
    nexout << QString("DIMENSIONS NCHAR = %1;\nCHARSTATELABELS\n").arg(nchar).toStdString();
    int ctr = 1;
    while (query.next()) {
        QString label;
        label = query.value(0).toString();
        nexout << QString(" %1 ").arg(ctr).toStdString();
        ctr++;
        nexout << NxsString::GetEscaped(label.toStdString());
        nexout << "\n";
    }

    nexout << ";\nEND;\n";

    nexout.close();
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

    qDebug() << "Selected: " << taxonID.toString();

    taxonFilter = taxonID.toString();

    observationsTable->setFilter(QString("taxon = '%1'").arg(taxonID.toString()));
    observationsTable->select();
}

void MainWindow::onObsFilterEdited(const QString &string)
{
    QSqlQuery query;
    query.exec(QString("SELECT char_GUUID FROM characters WHERE label LIKE '%") + string + QString("%' "));

    // This conditional clears the filter if no string is input (i.e. when a filter is erased)
    if (string == "") {
        observationsTable->setFilter("");
        observationsTable->select();
        return;
    }

    query.next();
    QString filter = "(observations.character = '";
    filter += (query.value(0).toString() + QString("' "));
    while (query.next()) {
        filter += QString(" OR observations.character = '");
        filter += (query.value(0).toString() + QString("'"));
    }
    filter += QString(")");

    if (taxonFilter != "") {
        filter += (QString(" AND (observations.taxon = '%1')").arg(taxonFilter));
    }

    qDebug() << filter;

    observationsTable->setFilter(filter);
    observationsTable->select();
}

void MainWindow::openCharTableView()
{
    CharacterEditorWindow *charEditor = new CharacterEditorWindow;
    connect(charTable, &QSqlRelationalTableModel::dataChanged, this, &MainWindow::onDataChanged);
    connect(stateTable, &QSqlRelationalTableModel::dataChanged, this, &MainWindow::onDataChanged);
    charEditor->show();
}

void MainWindow::openStateTableView()
{
    QWidget *statesWindow = new QWidget;

    QTableView *stateTableView = new QTableView;
    stateTableView->setModel(stateTable);

    statesWindow->setLayout(new QHBoxLayout);
    statesWindow->layout()->addWidget(stateTableView);

    stateTableView->resizeColumnsToContents();

    connect(stateTable, &QSqlRelationalTableModel::dataChanged, this, &MainWindow::onDataChanged);

    statesWindow->setMinimumWidth(stateTableView->horizontalHeader()->length());

    stateTable->select();
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
    QSqlQuery query;

    query.exec("CREATE TABLE taxa ("
                "taxon_id    INTEGER PRIMARY KEY AUTOINCREMENT,"
                "taxon_GUUID VARCHAR(7) UNIQUE,"
                "name        VARCHAR(100),"
                "included    INT(1),"
                "author      VARCHAR(100))");

    query.exec("CREATE TABLE characters ("
                "char_id    INTEGER PRIMARY KEY AUTOINCREMENT,"
                "char_GUUID VARCHAR(7) UNIQUE,"
                "label      MEDIUMTEXT,"
                "source     MEDIUMTEXT,"
                "included   INT(1),"
                "notes      MEDIUMTEXT)");

    query.exec("CREATE TABLE states ("
                "state_id   INTEGER PRIMARY KEY AUTOINCREMENT,"
                "symbol     VARCHAR(1),"
                "character  VARCHAR(7),"
                "label      VARCHAR(200),"
                "definition MEDIUMTEXT,"
                "UNIQUE (character, label),"
                "UNIQUE (character, symbol),"
                "FOREIGN KEY (character) REFERENCES characters (char_id))");

    query.exec("CREATE TABLE observations ("
                "id        INTEGER PRIMARY KEY AUTOINCREMENT,"
                "taxon     INTEGER,"
                "character INTEGER,"
                "state     INTEGER,"
                "notes     MEDIUMTEXT,"
                "FOREIGN KEY (taxon) REFERENCES taxa (taxon_id) ON UPDATE CASCADE,"
                "FOREIGN KEY (state) REFERENCES states (state_id) ON UPDATE CASCADE,"
                "FOREIGN KEY (character) REFERENCES characters (char_id) ON UPDATE CASCADE)");

    if (query.next()) {
        qDebug() << query.value(0).toString();
    }
}

void MainWindow::configMainTables()
{
    taxaTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    taxaTable->setTable("taxa");
    taxaTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);
    taxaTable->setHeaderData(0, Qt::Horizontal, tr("ID"));
    taxaTable->setHeaderData(1, Qt::Horizontal, tr("GUUID"));
    taxaTable->setHeaderData(2, Qt::Horizontal, tr("Name"));
    taxaTable->setHeaderData(3, Qt::Horizontal, tr("Author"));

    charTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    charTable->setTable("characters");
    charTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);

    stateTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    stateTable->setTable("states");
    stateTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);
    stateTable->setRelation(1, QSqlRelation("characters", "char_id", "label"));

    observationsTable = new QSqlRelationalTableModel(this, QSqlDatabase::database());
    observationsTable->setTable("observations");
    observationsTable->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);

    observationsTable->setRelation(1, QSqlRelation("taxa", "taxon_id", "name"));
    observationsTable->setRelation(2, QSqlRelation("characters", "char_id", "label"));
    observationsTable->setRelation(3, QSqlRelation("states", "state_id", "label"));

    observationsTable->setHeaderData(0, Qt::Horizontal, tr("ID"));
    observationsTable->setHeaderData(1, Qt::Horizontal, tr("Taxon"));
    observationsTable->setHeaderData(2, Qt::Horizontal, tr("Character"));
    observationsTable->setHeaderData(3, Qt::Horizontal, tr("State"));
    observationsTable->setHeaderData(4, Qt::Horizontal, tr("Notes"));

    taxaTableView->setModel(taxaTable);
    obsTableView->setModel(observationsTable);

    taxaTable->select();
    charTable->select();
    observationsTable->select();

    // Display the tables
    taxaTableView->setColumnHidden(0, true);
    taxaTableView->setColumnHidden(1, true);
//    //    taxaTableView->show();
    obsTableView->setColumnHidden(0, true);
    //    obsTableView->show();
    //    obsTableView->setSortingEnabled(true);

    taxaTableView->resizeColumnsToContents();
    obsTableView->resizeColumnsToContents();

    connect(taxaTableView, &QAbstractItemView::clicked, this, &MainWindow::onTaxonSelected);
    connect(obsFilterField, &QLineEdit::textEdited, this, &MainWindow::onObsFilterEdited);
    connect(observationsTable, &QSqlRelationalTableModel::dataChanged, obsTableView, &QTableView::resizeColumnsToContents);

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


