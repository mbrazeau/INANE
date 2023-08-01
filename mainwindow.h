#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>

class QPushButton;
class QTableView;
class QListView;
class QSqlRelationalTableModel;
class QLineEdit;
class MainMenu;
class QPlainTextEdit;

class MDatabaseManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void onDataChanged();
    void onTaxaChanged();
    void onTaxonSelected(const QModelIndex &index);
    void onObsFilterEdited(const QString &string);
    void dbNew();
    void dbOpen();
    void dbClose();
    void dbSave();
    void importNexus();
    void exportNexus();
    void openTaxaTableView();
    void openCharTableView();
    void openStateTableView();
    void aboutMenu();
    void writeToConsole(const QString &msg);
    void updateObsTable();

private:
    void createMenus();
    void showMessage(const QString &message, QMessageBox::Icon icon = QMessageBox::NoIcon);
    void showInitDialog();
    void createMainTables();
    void configMainTables();
    void resetMainTables();
    void getNewTaxon();
    void insertObservation();
    void clearFilters();

    MDatabaseManager *dbManager;

    QString taxonFilter;

    QLineEdit *obsFilterField;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *taxaMenu;
    QMenu *charsMenu;

    QSqlRelationalTableModel *taxaTable;
    QSqlRelationalTableModel *groupsTable;
    QSqlRelationalTableModel *charTable;
    QSqlRelationalTableModel *observationsTable;
    QSqlRelationalTableModel *stateTable;
    QSqlRelationalTableModel *symbolsTable;

    QTableView *taxaTableView;
    QListView  *taxaList;
    QTableView *charTableView;
    QTableView *obsTableView;
    QTableView *stateTableView;

    QPlainTextEdit *console;

    QWidget *statesWindow;

    QString path;

    MainMenu *mainMenu;

private slots:



signals:

};

#endif // MAINWINDOW_H
