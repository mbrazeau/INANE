#ifndef CHARACTEREDITORWINDOW_H
#define CHARACTEREDITORWINDOW_H

#include <QWidget>

class QHBoxLayout;
class QTableView;
class QSqlRelationalTableModel;
class QSqlDatabase;
class QSqlRelationalTableModel;

class CharacterEditorWindow : public QWidget
{
    Q_OBJECT

private:
    QHBoxLayout *charWindLayout;
    QTableView  *charTableView;
    QSqlRelationalTableModel *charTable_p;

public:
    explicit CharacterEditorWindow(QWidget *parent = nullptr);
    void setDatabase(QSqlRelationalTableModel *charTable);

signals:

};

#endif // CHARACTEREDITORWINDOW_H
