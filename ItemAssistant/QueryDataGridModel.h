#ifndef QUERYDATAGRIDMODEL_H
#define QUERYDATAGRIDMODEL_H

#include "datagrid/DataGridModel.h"
#include <Shared/IDB.h>


namespace aoia {

    class QueryDataGridModel
        : public DataGridModel
    {
    public:
        /// Constructor that takes the SQL used to load the data.
        QueryDataGridModel(sqlite::IDBPtr db, std::tstring const& sql);

        virtual ~QueryDataGridModel();

        /// Return number of columns.
        virtual unsigned int getColumnCount() const;

        /// Return the column header for a specific column.
        virtual std::tstring getColumnName(unsigned int index) const;

        /// Return the number of items.
        virtual unsigned int getItemCount() const;

        /// Return the name of a specified item.
        virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const;

    private:
        sqlite::IDBPtr m_db;
        sqlite::ITablePtr m_result;
    };
}

#endif // QUERYDATAGRIDMODEL_H
