#ifndef IDENTIFYLISTDATAMODEL_H
#define IDENTIFYLISTDATAMODEL_H

#include "DataGridModel.h"
#include <Shared/SQLite.h>


namespace aoia {

    class IdentifyListDataModel
        : public DataGridModel
    {
    public:
        IdentifyListDataModel();
        virtual ~IdentifyListDataModel();

        /// Return number of columns.
        virtual unsigned int getColumnCount() const;

        /// Return the column header for a specific column.
        virtual std::tstring getColumnName(unsigned int index) const;

        /// Return the number of items.
        virtual unsigned int getItemCount() const;

        /// Return the name of a specified item.
        virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const;

        /// Return the AOID of the specified item.
        unsigned int getItemId(unsigned int index) const;

    private:
        SQLite::TablePtr m_result;
    };

    typedef boost::shared_ptr<IdentifyListDataModel> IdentifyListDataModelPtr;
}

#endif // IDENTIFYLISTDATAMODEL_H
