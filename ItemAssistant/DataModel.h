#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "DataGridModel.h"
#include <Shared/SQLite.h>

namespace aoia { namespace sv {

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
        std::map<unsigned int, std::tstring> m_columns;
        unsigned int m_dimensionid;
        SQLite::TablePtr m_result;
    };

    typedef boost::shared_ptr<DataModel> DataModelPtr;

}}  // end of namespace

#endif // DATAMODEL_H
