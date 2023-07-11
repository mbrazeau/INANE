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

    QString taxonFilter;

    QLineEdit *obsFilterField;

    QMenu *fileMenu;
    QMenu *editMenu;
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

private slots:



signals:

};

#endif // MAINWINDOW_H
