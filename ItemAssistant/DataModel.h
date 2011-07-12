#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "DataGridModel.h"

namespace aoia { namespace sv {

    class DataModel
        : public DataGridModel
    {
    public:
        DataModel();
        virtual ~DataModel();

        virtual unsigned int getColumnCount() const;
        virtual std::tstring getColumnName(unsigned int index) const;
        virtual unsigned int getItemCount() const;
        virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const;

    private:
        std::map<unsigned int, std::tstring> m_columns;
    };

    typedef boost::shared_ptr<DataModel> DataModelPtr;

}}  // end of namespace

#endif // DATAMODEL_H
