#ifndef MDATABASEMANAGER_H
#define MDATABASEMANAGER_H

#include <QtSql>
#include <QMap>

class MDatabaseManager
{
public:
    MDatabaseManager();
    ~MDatabaseManager();

    static void createMainTables();

    QSqlRelationalTableModel *getTableModel(QString &modelName);
    int                       getId(QSqlRelationalTableModel &tableModel, QString &field, QModelIndex &index);
    void addStateToCharacter(const QString &label, int charID);
    bool hasDatabase();
    bool openDatabase(QString &dbname);

private:

    bool m_hasDatabase;

    QSqlRelationalTableModel *m_taxaTable;
    QSqlRelationalTableModel *m_groupsTable;
    QSqlRelationalTableModel *m_discreteCharsTable;
    QSqlRelationalTableModel *m_discreteStatesTable;
    QSqlRelationalTableModel *m_symbolsTable;
    QSqlRelationalTableModel *m_subcharsTable;
    QSqlRelationalTableModel *m_observationsTable;

    QSqlQuery query;
    QMap<QString, QSqlRelationalTableModel *> m_tableMap;
};

#endif // MDATABASEMANAGER_H
