#include <QSqlTableModel>
#include <QSqlQuery>

#include "compdatabase.h"

CompDataBase::CompDataBase()
{
//    query = new QSqlQuery(coreData);
//    query->exec("CREATE TABLE Taxa (id SERIAL primary key, taxon_name TINYTEXT)");
//    query->exec("CREATE TABLE Characters (id INT primary key, label MEDIUMTEXT, uuID varchar(64), notes LONGTEXT)");
//    query->exec("CREATE TABLE Observations (id INT primary key, taxon_name varchar(20), uuID varchar(64), short_label varchar(80), state varchar(20), notes varchar(20))");
}

bool CompDataBase::addTaxon(QString label)
{
//    query->exec(QString("INSERT INTO Taxa VALUES (1, %1)").arg(label));

    // TODO Check duplicates
}

bool CompDataBase::addCharacter(QString label)
{
//    query->exec(QString("INSERT INTO Characters VALUES (1, %1)").arg(label));

    // TODO Check duplicates
}



