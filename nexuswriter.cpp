#include <QtSql>

#include "nexuswriter.h"


NexusWriter::NexusWriter()
{

}

void NexusWriter::write(std::ofstream &nexout)
{
    QStringList taxaLabels;
    QVector<int> taxaIDs;
    QVector<int> charIDs;

    nexout << "#NEXUS\n\n";

    QSqlQuery query;

    nexout << "BEGIN TAXA;\n";

    query.exec(QString("SELECT COUNT(*) FROM taxa")); // TODO: add "WHERE included = 1"
    query.next();
    int ntax = query.value(0).toInt();
    nexout << QString("DIMENSIONS NTAX = %1;\nTAXLABELS\n").arg(ntax).toStdString();

    query.exec(QString("SELECT name, taxon_id FROM taxa"));
    while (query.next()) {
        QString label;
        taxaIDs.push_back(query.value("taxon_id").toInt());
        label = query.value("name").toString();
        nexout << NxsString::GetEscaped(label.toStdString());
        taxaLabels.push_back(QString::fromStdString(NxsString::GetEscaped(label.toStdString()))); // For later
        nexout << "\n";
    }

    nexout << ";\n";
    nexout << "END;\n\n";

    nexout << "BEGIN CHARACTERS;\n";
    query.exec(QString("SELECT COUNT(*) FROM characters")); // TODO: Condition this on included characters
    query.next();
    int nchar = query.value(0).toInt();
    query.exec(QString("SELECT char_id, charlabel FROM characters"));
    nexout << QString("DIMENSIONS NCHAR = %1;\nCHARLABELS\n").arg(nchar).toStdString();
    int ctr = 1;

    while (query.next()) {
        charIDs.push_back(query.value("char_id").toInt());
        QString label;
        label = query.value("charlabel").toString();
        nexout << QString(" %1 ").arg(ctr).toStdString();
        ctr++;
        nexout << NxsString::GetEscaped(label.toStdString());
        nexout << "\n";
    }

    assert(charIDs.size() == nchar);

    nexout << ";\n";

    // Generate the Nexus symbols expression based on actual symbols used in the dataset
    if(!query.exec("SELECT symbols.symbol "
                    "FROM symbols "
                    "WHERE symbol_id IN (SELECT DISTINCT states.symbol FROM states) "
                    "AND symbols.symbol != '?' AND symbols.symbol != '-'")) {
        qDebug() << query.lastError().text();
    }

    QString symbols;
    while (query.next()) {
        symbols += query.value(0).toString();
        symbols += " ";
    }

    nexout << QString("FORMAT DATATYPE=STANDARD GAP=- MISSING=? SYMBOLS=\"%1\";\n").arg(symbols).toStdString();

    nexout << "MATRIX\n";

    // This is the tricky part: format the matrix
    QSqlQueryModel qmodel;
    for (int i = 0; i < taxaIDs.size(); ++i) {
        QVector<int> scoreCtr;

        nexout << taxaLabels[i].toStdString();
        nexout << "\t";

        qDebug() << "Taxon: " << taxaIDs[i];
        qmodel.setQuery(QString("SELECT taxon, character, state, symbols.symbol "
                                 "FROM observations "
                                 "INNER JOIN symbols "
                                 "ON (SELECT symbol FROM states WHERE state_id = observations.state) = symbols.symbol_id "
                                 "WHERE taxon = %1").arg(taxaIDs[i]));

        // We need to ensure that the program outputs the column-ordered sequence of characters consistently for
        // all characters in the dataset, including the correct handling of polymorphisms. This means going through
        // the query result by character ID in each case. But we also need to count the number of times each character
        // appears in the results. It's a pain but needs to be done.
        qDebug() << "Expected number of characters: " << charIDs.size();
        int j;
        for (j = 0; j < charIDs.size(); ++j) {

            scoreCtr.clear();
            // This loop counts the number of times this particular character appears in the matrix
            for (int k = 0; k < qmodel.rowCount(); ++k) {
                if (qmodel.record(k).value("character").toInt() == charIDs[j]) {
                    scoreCtr.push_back(k);
                }
            }

            for (int k = 0; k < scoreCtr.size(); ++k) {
                if (scoreCtr.length() > 1 && k == 0) {
                    nexout << "{";
                }

                qmodel.fetchMore();
                nexout << qmodel.record(scoreCtr[k]).value("symbol").toString().toStdString();

                if (scoreCtr.size() > 1) {
                    if (k == (scoreCtr.length()-1)) {
                        nexout << "}";
                    } else {
                        nexout << " ";
                    }
                }
            }
        }

        qDebug() << "Number processed: " << j;
        std::cout << "\n";
        nexout << "\n";
    }

    nexout << ";\n";
    nexout << "END;\n\n";
}

QString NexusWriter::writeMatrix()
{
    QSqlQueryModel *qmodel = new QSqlQueryModel;
    QVector<int> idlist;
    QStringList names;

    qmodel->setQuery("SELECT taxon_id, name FROM taxa");
    int i;
    for (i = 0; i < qmodel->rowCount(); ++i) {
        idlist.push_back(qmodel->record(i).value("taxon_id").toInt());
        names.push_back(qmodel->record(i).value("name").toString());
    }


    for (i = 0; i < idlist.length(); ++i) {
        qmodel->setQuery(QString("SELECT state FROM observations WHERE taxon = %1").arg(idlist.at(i)));
    }

    delete qmodel;
}
