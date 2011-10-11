#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include "DataGridModel.h"

namespace aoia
{
    class CSVExporter
    {
    public:
        CSVExporter();
        ~CSVExporter();

        void DoExport(std::tostream &os, DataGridModelPtr data) const;

    protected:
        std::tstring EnsureEncapsulation(std::tstring const& field) const;
    };
}

#endif // CSVEXPORTER_H
