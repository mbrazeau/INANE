#include "mdatabasemanager.h"

MDatabaseManager::MDatabaseManager()
{

}

void MDatabaseManager::createMainTables()
{
    QSqlQuery query;

    query.exec("CREATE TABLE taxa ("
               "taxon_id    INTEGER PRIMARY KEY,"
               "taxon_GUUID VARCHAR(7) UNIQUE,"
               "name        VARCHAR(100),"
               "otu         VARCHAR(100),"
               "taxgroup    INTEGER,"
               "included    INT(1),"
               "author      VARCHAR(100),"
               "FOREIGN KEY (taxgroup) REFERENCES taxongroups (group_id))");

    query.exec("CREATE TABLE taxongroups ("
               "group_id    INTEGER PRIMARY KEY,"
               "groupname   VARCHAR(100))");

    query.exec("CREATE TABLE characters ("
               "char_id    INTEGER PRIMARY KEY,"
               "char_GUUID VARCHAR(7) UNIQUE,"
               "label      MEDIUMTEXT,"
               "source     MEDIUMTEXT,"
               "included   INT(1),"
               "notes      MEDIUMTEXT)");

    query.exec("CREATE TABLE states ("
               "state_id   INTEGER PRIMARY KEY,"
               "symbol     VARCHAR(1),"
               "character  INTEGER,"
               "label      VARCHAR(200),"
               "definition MEDIUMTEXT,"
               "UNIQUE (character, label),"
               "UNIQUE (character, symbol),"
               "FOREIGN KEY (character) REFERENCES characters (char_id))");

    query.exec("CREATE TABLE observations ("
               "id        INTEGER PRIMARY KEY,"
               "taxon     INTEGER,"
               "character INTEGER,"
               "state     INTEGER,"
               "notes     MEDIUMTEXT,"
//                "UNIQUE (taxon, character, state),"
               "FOREIGN KEY (taxon) REFERENCES taxa (taxon_id) ON UPDATE CASCADE,"
               "FOREIGN KEY (state) REFERENCES states (state_id) ON UPDATE CASCADE,"
               "FOREIGN KEY (character) REFERENCES characters (char_id) ON UPDATE CASCADE)");
}

int MDatabaseManager::getId(QSqlRelationalTableModel &tableModel, QString &field, QModelIndex &index)
{

    return -1;
}

