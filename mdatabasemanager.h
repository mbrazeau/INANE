#ifndef MDATABASEMANAGER_H
#define MDATABASEMANAGER_H

#include <QtSql>

class MDatabaseManager
{
public:
    MDatabaseManager();

    void createMainTables();

    QSqlRelationalTableModel *getModel(QString &modelName);
    int                       getId(QSqlRelationalTableModel &tableModel, QString &field, QModelIndex &index);

private:

    QSqlRelationalTableModel *m_taxaTable;
    QSqlRelationalTableModel *m_groupsTable;
    QSqlRelationalTableModel *m_discreteCharsTable;
    QSqlRelationalTableModel *m_discreteStatesTable;
    QSqlRelationalTableModel *m_subcharsTable;
    QSqlRelationalTableModel *m_observationsTable;

};

#endif // MDATABASEMANAGER_H
