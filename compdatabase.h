#ifndef COMPDATABASE_H
#define COMPDATABASE_H

#include <QSqlDatabase>

class CompDataBase
{
public:
    CompDataBase();

    QSqlDatabase coreData;


    bool addTaxon(QString label);
    bool addCharacter(QString label);

//    QSqlQuery       *query;
//    QSqlDatabase    &database();
};

#endif // COMPDATABASE_H
