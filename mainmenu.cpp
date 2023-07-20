#include <QMenuBar>
#include <QList>

#include "mainwindow.h"
#include "mainmenu.h"

MainMenu::MainMenu(MainWindow *parent) : QMenuBar(parent)
{
    dataMenuActions.clear();
    inFileActionsList.clear();

    // File menu
    m_fileMenu = addMenu(tr("&File"));

    m_fileNewDb = new QAction(tr("&New Database..."), this);
    m_fileMenu->addAction(m_fileNewDb);
    m_fileNewDb->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    inFileActionsList.push_back(m_fileNewDb);
    connect(m_fileNewDb, &QAction::triggered, parent, &MainWindow::dbNew);

    m_fileOpenDb = new QAction(tr("&Open Database..."), this);
    m_fileMenu->addAction(m_fileOpenDb);
    m_fileOpenDb->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    inFileActionsList.push_back(m_fileOpenDb);
    connect(m_fileOpenDb, &QAction::triggered, parent, &MainWindow::dbOpen);

    m_fileSaveDb = new QAction(tr("&Save Database..."), this);
    m_fileMenu->addAction(m_fileSaveDb);
    m_fileSaveDb->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    dataMenuActions.push_back(m_fileSaveDb);
//    connect(m_fileSaveDb, &QAction::triggered, parent, &MainWindow::dbSave);

    m_fileCloseDb = new QAction(tr("&Close Database..."), this);
    m_fileMenu->addAction(m_fileCloseDb);
    connect(m_fileCloseDb, &QAction::triggered, parent, &MainWindow::dbClose);

    m_fileMenu->addSeparator();

    //    m_fileImport = new QMenu(tr("Import"), this);
    m_fileImport = m_fileMenu->addMenu(tr("&Import"));

    m_fileImportNex = new QAction(tr("&Nexus..."), this);
    m_fileImport->addAction(m_fileImportNex);
    inFileActionsList.push_back(m_fileImportNex);
    connect(m_fileImportNex, &QAction::triggered, parent, &MainWindow::importNexus);

    m_fileExport = m_fileMenu->addMenu(tr("Export"));
    m_fileExportNex = new QAction(tr("&Nexus..."), this);
    m_fileExport->addAction(m_fileExportNex);
    connect(m_fileExportNex, &QAction::triggered, parent, &MainWindow::exportNexus);
    dataMenuActions.push_back(m_fileExportNex);

    // Edit menu

    // Taxa menu
    m_taxaMenu = addMenu(tr("&Taxa"));
    m_taxaEdit = new QAction(tr("&Edit taxa..."), this);
    m_taxaMenu->addAction(m_taxaEdit);
    dataMenuActions.push_back(m_taxaEdit);

    // Characters menu
    m_charsMenu = addMenu(tr("&Characters"));
    m_charsEdit = new QAction(tr("Edit characters..."), this);
    m_charsMenu->addAction(m_charsEdit);
    dataMenuActions.push_back(m_charsEdit);
    connect(m_charsEdit, &QAction::triggered, parent, &MainWindow::openCharTableView);

    // TEMPORARY
    QAction *m_statesEdit = new QAction(tr("Edit states..."), this);
    m_charsMenu->addAction(m_statesEdit);
    dataMenuActions.push_back(m_statesEdit);
    connect(m_statesEdit, &QAction::triggered, parent, &MainWindow::openStateTableView);
//    openStateTableView
    // END TEMP

    // Help
    m_helpMenu = addMenu(tr("&Help"));

    m_about = new QAction(tr("&About"), this);
    m_helpMenu->addAction(m_about);
    connect(m_about, &QAction::triggered, parent, &MainWindow::aboutMenu);
}

void MainMenu::setDataMenusEnabled(bool enable)
{
    QListIterator<QAction *> it(dataMenuActions);
    while (it.hasNext()) {
        it.next()->setEnabled(enable);
    }
}

void MainMenu::setInFileMenusEnabled(bool enable)
{
    QListIterator<QAction *> it(inFileActionsList);
    while (it.hasNext()) {
        it.next()->setEnabled(enable);
    }
}
