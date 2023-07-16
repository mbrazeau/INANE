#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQuery>

class QPushButton;
class QTableView;
class QListView;
class QSqlRelationalTableModel;
class QLineEdit;
class MainMenu;
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
    void openCharTableView();
    void aboutMenu();

private:
    void createMenus();
    void showMessage(QString message);
    void openStateTableView();
    void showInitDialog();
    void createMainTables();
    void configMainTables();

    QString taxonFilter;

    QLineEdit *obsFilterField;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *taxaMenu;
    QMenu *charsMenu;

    QSqlDatabase db;

    QSqlRelationalTableModel *taxaTable;
    QSqlRelationalTableModel *charTable;
    QSqlRelationalTableModel *observationsTable;
    QSqlRelationalTableModel *stateTable;

    QTableView *taxaTableView;
    QListView  *taxaList;
    QTableView *charTableView;
    QTableView *obsTableView;
    QTableView *stateTableView;

    QString path;

    MainMenu *mainMenu;

private slots:



signals:

};

#endif // MAINWINDOW_H
