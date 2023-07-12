#include "nexusreader.h"

NexusReader::NexusReader()
{
    // Allocate the storage for blocks
    _reader         = new MultiFormatReader();
    _taxa           = new NxsTaxaBlock();
    _assumptions    = new NxsAssumptionsBlock(_taxa);
    _characters     = new NxsCharactersBlock(_taxa, _assumptions);
    _datab          = new NxsDataBlock(_taxa, _assumptions);

   // Add the blocks to the reader stack
    _reader->Add(_taxa);
    _reader->Add(_assumptions);
    _reader->Add(_characters);
    _reader->Add(_datab);
}

void NexusReader::openNexusFile(char *arg)
{
    _reader->ReadFilepath(arg, MultiFormatReader::NEXUS_FORMAT);

    if (hasDataBlock()) {
        _datab->TransferTo(*_characters);
    }
}

void NexusReader::closeNexusFile()
{
    // TODO: Ask to save first, of course.

    _reader->DeleteBlocksFromFactories();
    return;
}

void NexusReader::saveNexusFile()
{
    return;
}

unsigned int NexusReader::getNchar()
{
    return _characters->GetNChar();
}

unsigned int NexusReader::getNumStatesForChar(unsigned int i)
{
    return _characters->GetObsNumStates(i, true);
}

int NexusReader::getInternalRepresentation(unsigned int i, unsigned int j, unsigned int k)
{
    _characters->GetInternalRepresentation(i, j, k);
}

unsigned int NexusReader::getNtax()
{
    return _taxa->GetNTax();
}

QString NexusReader::getTaxLabel(unsigned int i)
{
    NxsString nxTaxlabel = _taxa->GetTaxonLabel(i);

    QString *ret = new QString(nxTaxlabel.c_str());

    return *ret;
}

QString NexusReader::getCharLabel(unsigned int char_i)
{
    NxsString nxCharLabel = _characters->GetCharLabel(char_i);
    QString *ret = new QString(nxCharLabel.c_str());

    return *ret;
}

QString NexusReader::getStateLabel(unsigned int char_i, unsigned int state_i)
{
    NxsString nxStateLabel = _characters->GetStateLabel(char_i, state_i);
    QString *ret = new QString(nxStateLabel.c_str());

    return *ret;
}

unsigned int NexusReader::getNumStates(unsigned taxon_i, unsigned char_i)
{
    if (_characters->GetInternalRepresentation(taxon_i, char_i, 0) < 0) {
        return 1;
    }

    return _characters->GetNumStates(taxon_i, char_i);
}

QString NexusReader::getStateLabel(unsigned int taxon_i, unsigned int char_i, unsigned int state_i)
{
    unsigned int i;

    if (_characters->GetInternalRepresentation(taxon_i, char_i, 0) == -2) {
        return "missing";
    }

    if (_characters->GetInternalRepresentation(taxon_i, char_i, 0) == -3) {
        return "inapplicable";
    }

    return QString(_characters->GetStateLabel(char_i, state_i).c_str());
}


bool NexusReader::hasDataBlock()
{
    BlockReaderList b1 = _reader->GetUsedBlocksInOrder();
    for (std::list<NxsBlock *>::iterator i = b1.begin(); i != b1.end(); ++i)
    {
        NxsBlock * b = (*i);
        if (b->GetID().compare("DATA") == 0)
        {
            return true;
        }
    }
    return false;
}

QString NexusReader::getCellData(unsigned int tax_i, unsigned int char_i)
{
    QString c;

    unsigned int i = 0;

    if (_characters->GetInternalRepresentation(tax_i, char_i, 0) == -2) {
        return "?";
    }

    for (i = 0; i < _characters->GetNumStates(tax_i, char_i); ++i) {
        if (i > 0) {
            if (_characters->IsPolymorphic(tax_i, char_i)) {
                c.append("&");
            } else {
                c.append("/");
            }
        }
        c.append(_characters->GetState(tax_i, char_i, i));
    }


    return c;
}
