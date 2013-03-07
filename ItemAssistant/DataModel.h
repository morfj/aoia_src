#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "datagrid/DataGridModel.h"
#include <shared/IDB.h>


namespace aoia
{
    namespace sv
    {

        struct DataModelItem
        {
            unsigned int charid;
            std::tstring name;
            std::map<unsigned int, std::tstring> stats;
        };

        /**
         * Queries all summary data for all toons in the specified dimension.
         */
        class DataModel
            : public DataGridModel
        {
        public:
            DataModel(sqlite::IDBPtr db);
            virtual ~DataModel();

            // 'DataGridModel' implementation
            virtual unsigned int getColumnCount() const;
            virtual std::tstring getColumnName(unsigned int index) const;
            virtual unsigned int getItemCount() const;
            virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const;

        private:
            sqlite::IDBPtr m_db;
            sqlite::ITablePtr m_result;
            std::map<unsigned int, std::tstring> m_columnTitles;
            std::vector<unsigned int> m_statids;
            std::vector<DataModelItem> m_items;
        };

        typedef boost::shared_ptr<DataModel> DataModelPtr;

    }   // namespace sv
}  // namespace aoia

#endif // DATAMODEL_H
