#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenuBar>
#include <QList>

class MainWindow;
class MainMenu : public QMenuBar
{
    Q_OBJECT
public:
    MainMenu(MainWindow *parent);

    void setDataMenusEnabled(bool);
    void setInFileMenusEnabled(bool);

private:

    QList<QAction *> dataMenuActions;
    QList<QAction *> inFileActionsList;
    // File menu
    QMenu   *m_fileMenu;
    QAction *m_fileNewDb;
    QAction *m_fileOpenDb;
    QAction *m_fileSaveDb;
    QAction *m_fileCloseDb;
    QMenu   *m_fileImport;
    QAction *m_fileImportNex;
    QMenu   *m_fileExport;
    QAction *m_fileExportNex;

    // Edit menu
    QMenu *m_editMenu;

    // Taxa menu
    QMenu *m_taxaMenu;
    QAction *m_taxaEdit;

    // Characters menu
    QMenu *m_charsMenu;
    QAction *m_charsEdit;

    // Project menu
    QMenu *m_projectMenu;
    QAction *m_projNew;
    QAction *m_projGenGUUIDs;
    QAction *m_projPublish;

    // Help
    QMenu *m_helpMenu;

    // About
    QAction *m_about;
};

#endif // MAINMENU_H
