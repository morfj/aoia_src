#include "StdAfx.h"
#include "DataModel.h"
#include <boost/assign.hpp>
#include "DBManager.h"
#include "Shared/SQLite.h"

namespace aoia { namespace sv {

    DataModel::DataModel(unsigned int dimensionid)
        : m_dimensionid(dimensionid)
    {
        // Sets up the map between column-index and title.
        m_columnTitles = boost::assign::map_list_of
            (0, _T("Toon"))
            (1, _T("Levels"))
            (2, _T("AI Levels"))
            (3, _T("LE Levels"))
            (4, _T("Credits"));

        SQLite::TablePtr toons = g_DBManager.ExecTable(STREAM2STR("SELECT charid, charname FROM tToons WHERE dimensionid = " << m_dimensionid));
        for (unsigned int i = 0; i < toons->Rows(); ++i)
        {
            DataModelItem item;
            item.charid = boost::lexical_cast<unsigned int>(toons->Data(i, 0));
            item.name = from_ascii_copy(toons->Data(i, 1));

            SQLite::TablePtr stats = g_DBManager.ExecTable(STREAM2STR("SELECT statid, statvalue FROM tToonStats WHERE charid = " << item.charid << " AND statid IN(54, 169, 263, 61)"));
            for (unsigned int j = 0; j < stats->Rows(); ++j)
            {
                unsigned int id = boost::lexical_cast<unsigned int>(stats->Data(j, 0));
                item.stats[id] = from_ascii_copy(stats->Data(j, 1));
            }

            m_items.push_back(item);
        }
    }

    DataModel::~DataModel()
    {
    }

    unsigned int DataModel::getColumnCount() const
    {
        if (m_items.empty()) {
            return 0;
        }

        return m_columnTitles.size();
    }

    std::tstring DataModel::getColumnName( unsigned int index ) const
    {
        if (m_items.empty()) {
            return _T("");
        }

        std::map<unsigned int, std::tstring>::const_iterator it = m_columnTitles.find(index);
        if (it != m_columnTitles.end())
        {
            return it->second;
        }

        LOG("aoia::sv::DataModel::getColumnName() : Invalid index supplied.");
        return _T("");
    }

    unsigned int DataModel::getItemCount() const
    {
        if (m_items.empty()) {
            return 0;
        }

        return m_items.size();
    }

    std::tstring DataModel::getItemProperty( unsigned int index, unsigned int column ) const
    {
        static unsigned int statid[] = { 54, 169, 263, 61 };
        if (m_items.empty()) {
            return _T("");
        }

        DataModelItem const& item = m_items.at(index);

        if (column == 0) {
            return item.name;
        }
        --column;

        std::map<unsigned int, std::tstring>::const_iterator it = item.stats.find(statid[column]);
        if (it != item.stats.end()) {
            return it->second;
        }
        return _T("-");
    }

}}  // end of namespace
