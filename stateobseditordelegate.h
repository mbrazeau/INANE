#ifndef STATEOBSEDITORDELEGATE_H
#define STATEOBSEDITORDELEGATE_H

#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QObject>

class StateObsEditorDelegate : public QSqlRelationalDelegate
{
    Q_OBJECT

private:

    QSqlRelationalTableModel *m_statesTable;

public:
    StateObsEditorDelegate(QWidget *parent);
    void setStateTable(QSqlRelationalTableModel &statesTable);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

#endif // STATEOBSEDITORDELEGATE_H
