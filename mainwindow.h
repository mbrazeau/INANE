#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQuery>

class QPushButton;
class QTableView;
class QListView;
class QSqlRelationalTableModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void createMenus();
    void aboutMenu();
    void showMessage(QString message);
    void dbNew();
    void dbOpen();
    void dbClose();
    void importNexus();
    void openCharTableView();
    void openStateTableView();
    void showInitDialog();
    void filterObsTable(QString filterValue);
    void createMainTables();
    void configMainTables();
//    void onDataChanged();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *charsMenu;

    QSqlDatabase *centralDb;

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

private slots:
    void onDataChanged();
    void onTaxaChanged();
    void onTaxonSelected(const QModelIndex &index);
    void onObsFilterEdited(const QString &string);


signals:

};

#endif // MAINWINDOW_H
