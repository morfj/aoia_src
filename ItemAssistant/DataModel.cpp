#include "StdAfx.h"
#include "DataModel.h"
#include <boost/assign.hpp>

namespace aoia { namespace sv {

    DataModel::DataModel()
    {
        // Sets up the map between column-index and title.
        m_columns = boost::assign::map_list_of
            (0, _T("Toon"))
            (1, _T("Levels"))
            (2, _T("Alien Levels"))
            (3, _T("Research Levels"))
            (4, _T("Credits"));
    }

    DataModel::~DataModel()
    {
    }

    unsigned int DataModel::getColumnCount() const
    {
        return 5;
    }

    std::tstring DataModel::getColumnName( unsigned int index ) const
    {
        std::map<unsigned int, std::tstring>::const_iterator it = m_columns.find(index);
        if (it != m_columns.end())
        {
            return it->second;
        }

        return _T("");
    }

    unsigned int DataModel::getItemCount() const
    {
        return 0;
    }

    std::tstring DataModel::getItemProperty( unsigned int index, unsigned int column ) const
    {
        return _T("");
    }

}}  // end of namespace
