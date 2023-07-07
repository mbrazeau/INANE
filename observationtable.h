#ifndef OBSERVATIONTABLE_H
#define OBSERVATIONTABLE_H

#include <QObject>

class ObservationTable //: public QxtCsvModel
{
//    Q_OBJECT
public:
    explicit ObservationTable(QObject *parent = nullptr);

private:
    QStringList m_headerTitles = {
        "TaxonID",
        "Taxon Name",
        "CharID",
        "Character Label",
        "State",
        "Notes"
    };
};

#endif // OBSERVATIONTABLE_H
