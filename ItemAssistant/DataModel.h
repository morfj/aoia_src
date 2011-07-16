#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "DataGridModel.h"
#include <Shared/SQLite.h>

namespace aoia { namespace sv {

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
        DataModel(unsigned int dimensionid);
        virtual ~DataModel();

        // 'DataGridModel' implementation
        virtual unsigned int getColumnCount() const;
        virtual std::tstring getColumnName(unsigned int index) const;
        virtual unsigned int getItemCount() const;
        virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const;

        unsigned int getDimensionId() const { return m_dimensionid; }

    private:
        std::map<unsigned int, std::tstring> m_columnTitles;
        std::vector<unsigned int> m_statids;
        unsigned int m_dimensionid;
        SQLite::TablePtr m_result;
        std::vector<DataModelItem> m_items;
    };

    typedef boost::shared_ptr<DataModel> DataModelPtr;

}}  // end of namespace

#endif // DATAMODEL_H
