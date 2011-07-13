#include "StdAfx.h"
#include "DataModel.h"
#include <boost/assign.hpp>
#include "DBManager.h"

namespace aoia { namespace sv {

    DataModel::DataModel(unsigned int dimensionid)
        : m_dimensionid(dimensionid)
    {
        // Sets up the map between column-index and title.
        m_columns = boost::assign::map_list_of
            (0, _T("Toon"))
            (1, _T("Levels"))
            (2, _T("Alien Levels"))
            (3, _T("Research Levels"))
            (4, _T("Credits"));

        m_result = g_DBManager.ExecTable(STREAM2STR("SELECT charname, credits FROM tToons WHERE dimensionid = " << m_dimensionid));
    }

    DataModel::~DataModel()
    {
    }

    unsigned int DataModel::getColumnCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return m_columns.size();
    }

    std::tstring DataModel::getColumnName( unsigned int index ) const
    {
        if (!m_result)
        {
            return _T("");
        }

        std::map<unsigned int, std::tstring>::const_iterator it = m_columns.find(index);
        if (it != m_columns.end())
        {
            return it->second;
        }

        LOG("aoia::sv::DataModel::getColumnName() : Invalid index supplied.");
        return _T("");
    }

    unsigned int DataModel::getItemCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return m_result->Rows();
    }

    std::tstring DataModel::getItemProperty( unsigned int index, unsigned int column ) const
    {
        if (!m_result)
        {
            return _T("");
        }

        return from_ascii_copy(m_result->Data(index, column));
    }

}}  // end of namespace
