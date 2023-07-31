#include "mdatabasemanager.h"

const char stateSymbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-?";

MDatabaseManager::MDatabaseManager() :
    m_hasDatabase(false)
{
    // First check that a valid database is active
    // TODO: do the above

    // Allocate the tables
    QString tableName;
    m_taxaTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "taxa";
    m_taxaTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_taxaTable);

    m_groupsTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "taxongroups";
    m_groupsTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_groupsTable);

    m_discreteCharsTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "characters";
    m_discreteCharsTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_discreteCharsTable);

    m_discreteStatesTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "states";
    m_discreteStatesTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_discreteStatesTable);

    m_symbolsTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "symbols";
    m_symbolsTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_symbolsTable);

    m_subcharsTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "characters";
    m_subcharsTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_subcharsTable);

    m_observationsTable = new QSqlRelationalTableModel(nullptr, QSqlDatabase::database());
    tableName = "observations";
    m_observationsTable->setTable(tableName);
    m_tableMap.insert(tableName.toLower(), m_observationsTable);

    // Define the relations
    m_taxaTable->setRelation(4, QSqlRelation("taxongroups", "group_id", "groupname"));
    m_discreteStatesTable->setRelation(1, QSqlRelation("characters", "char_id", "charlabel"));
    m_observationsTable->setRelation(1, QSqlRelation("taxa", "taxon_id", "name"));
    m_observationsTable->setRelation(2, QSqlRelation("characters", "char_id", "charlabel"));
    m_observationsTable->setRelation(3, QSqlRelation("states", "state_id", "statelabel"));
}

MDatabaseManager::~MDatabaseManager()
{
    QString connection = QSqlDatabase::database().databaseName();
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase(connection);

    delete m_taxaTable;
    delete m_groupsTable;
    delete m_discreteCharsTable;
    delete m_discreteStatesTable;
    delete m_symbolsTable;
    delete m_subcharsTable;
    delete m_observationsTable;
}

void MDatabaseManager::createMainTables()
{
    QSqlQuery query;

    QSqlDatabase::database().transaction();
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

    query.exec("INSERT INTO taxongroups (groupname) VALUES ('default group')");

    query.exec("CREATE TABLE characters ("
               "char_id    INTEGER PRIMARY KEY,"
               "char_GUUID VARCHAR(7) UNIQUE,"
               "charlabel  MEDIUMTEXT,"
               "source     MEDIUMTEXT,"
               "included   INT(1),"
               "notes      MEDIUMTEXT)");

    query.exec("CREATE TABLE states ("
               "state_id   INTEGER PRIMARY KEY,"
               "symbol     VARCHAR(20),"
               "character  INTEGER,"
               "statelabel VARCHAR(200),"
               "definition MEDIUMTEXT,"
               "UNIQUE (character, statelabel),"
               "UNIQUE (character, symbol),"
               "FOREIGN KEY (character) REFERENCES characters (char_id),"
               "FOREIGN KEY (symbol) REFERENCES symbols (symbol_id))");

    query.exec("CREATE TABLE symbols ("
               " symbol_id INTEGER PRIMARY KEY,"
               " rank      INTEGER,"
               " symbol    VARCHAR(20),"
               " UNIQUE (symbol)"
               ")");

    for (int i = 0; i < strlen(stateSymbols); ++i) {
        QString symb = QChar(stateSymbols[i]);
        query.prepare("INSERT INTO symbols (rank, symbol) VALUES (:rank, :symbol)");
        query.bindValue(":rank", i + 1);
        query.bindValue(":symbol", symb);
        if(!query.exec()){
            qDebug() << query.lastError().text();
        }
    }

    query.exec("INSERT INTO states (statelabel, character, symbol) "
               "VALUES "
               "('missing', NULL, (SELECT symbol_id FROM symbols WHERE symbol = '?')),"
               "('inapplicable',NULL, (SELECT symbol_id FROM symbols WHERE symbol = '-'))");

    query.exec("CREATE TABLE subchars ("
               "subchar_id INTEGER PRIMARY KEY,"
               "parent     INTEGER,"
               "subchar    INTEGER"
               ")");

    query.exec("CREATE TABLE observations ("
               "id        INTEGER PRIMARY KEY,"
               "taxon     INTEGER,"
               "character INTEGER,"
               "state     INTEGER,"
               "notes     MEDIUMTEXT,"
               "UNIQUE (taxon, character, state),"
               "FOREIGN KEY (taxon) REFERENCES taxa (taxon_id) ON UPDATE CASCADE,"
               "FOREIGN KEY (state) REFERENCES states (state_id) ON UPDATE CASCADE,"
               "FOREIGN KEY (character) REFERENCES characters (char_id) ON UPDATE CASCADE)");

    if (query.next()) {
        qDebug() << query.value(0).toString();
    }

    QSqlDatabase::database().commit();
}

QSqlRelationalTableModel *MDatabaseManager::getTableModel(const QString &modelName)
{
    // TODO: Need to test this

    return m_tableMap[modelName.toLower()];
}

const QSqlRelationalTableModel *MDatabaseManager::getTableModel(const QModelIndex &index)
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    return sqlModel;
}

int MDatabaseManager::getId(QSqlRelationalTableModel &tableModel, QString &field, QModelIndex &index)
{
    return -1;
}

void MDatabaseManager::addStateToCharacter(const QString &label, int charID)
{
    query.prepare(QString("INSERT INTO states (statelabel, character) VALUES (:label, :char_id)"));
    if (label != "") {
        query.bindValue(":label", label);
    } else {
        query.bindValue(":label", "new state");
    }
    query.bindValue(":char_id", charID);
}

bool MDatabaseManager::hasDatabase()
{
    return m_hasDatabase;
}

bool MDatabaseManager::openDatabase(QString &dbname)
{

    if (QSqlDatabase::database().connectionName() != "") {
        qDebug() << "A connection \"" << QSqlDatabase::database().connectionName() << "\" already exists!";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    if (db.databaseName() != "") {
        qDebug() << "A database \"" << db.databaseName() << "\" is already open!";
    }

    db.setDatabaseName(dbname);
    if (!db.open()) {
        qDebug() << "No database!";
        return false;
    }

    dbName = dbname;

    return true;
}

void MDatabaseManager::addObservation(const int taxID, const int charID, const int stateID)
{
    QSqlQuery query;

    query.prepare("INSERT INTO observations (taxon, character, state) "
                  "VALUES (:taxID, :charID, :stateID)");
    query.bindValue(":taxID", taxID);
    query.bindValue(":charID", charID);
    query.bindValue(":stateID", stateID);
    if(!query.exec()) {
        qDebug() << query.lastError().text();
    }
}

void MDatabaseManager::addTaxon(const QString &name)
{
    QSqlQuery query;
    QCryptographicHash charUUID(QCryptographicHash::Sha1);
    QByteArray data;
    QByteArray hashresult;

    data = name.toLocal8Bit();
    charUUID.addData(data);
    hashresult = charUUID.result();
    charUUID.reset();
    QString shortHash = QString(hashresult.toHex().remove(0, 2 * hashresult.size() - 7));

    query.prepare(QString("INSERT INTO taxa (taxon_GUUID, name, otu, taxgroup, included) "
                          "VALUES (:taxon_id, :name, :name, (SELECT group_id FROM taxongroups WHERE groupname = 'default group'), 1)"));
    query.bindValue(":taxon_id", shortHash);
    query.bindValue(":name", name);
    query.exec();
    if (query.next()) {
        qDebug() << query.value(0).toString();
    }
}

void MDatabaseManager::addCharacter(const QString &label)
{

}

