#ifndef NEXUSWRITER_H
#define NEXUSWRITER_H

#include <QString>

#include <iostream>

#include "ncl.h"

class NexusWriter
{
public:
    NexusWriter();

    static void write(std::ofstream &nexout);

private:

    static QString writeMatrix();
};

#endif // NEXUSWRITER_H
