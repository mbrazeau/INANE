#ifndef NEXUSREADER_H
#define NEXUSREADER_H

#include <QString>
#include "ncl.h"

class NexusReader
{
public:
    NexusReader();

    void openNexusFile(char* arg);
    void closeNexusFile();
    void saveNexusFile();

    unsigned int getNtax();
    unsigned int getNchar();
    unsigned int getNumStatesForChar(unsigned int i);
    int getInternalRepresentation(unsigned int i, unsigned int j, unsigned int k);

    QString getCellData(unsigned int tax_i, unsigned int char_i);
    QString getTaxLabel(unsigned int i);
    QString getCharLabel(unsigned int char_i);
    QString getStateLabel(unsigned int char_i, unsigned int state_i);
    unsigned int getNumStates(unsigned taxon_i, unsigned char_i);
    QString getStateLabel(unsigned int taxon_i, unsigned int char_i, unsigned int state_i);

private:

    MultiFormatReader   *_reader;
    NxsTaxaBlock        *_taxa;
    NxsAssumptionsBlock *_assumptions;
    NxsCharactersBlock  *_characters;
    NxsDataBlock        *_datab;

    bool hasDataBlock();
};

#endif // NEXUSREADER_H
