#include "StdAfx.h"
#include "ItemListDataModel.h"
#include "DBManager.h"


namespace aoia {

    ItemListDataModel::ItemListDataModel(std::set<unsigned int> const& aoids)
    {
        std::ostringstream aoid_array;
        for (std::set<unsigned int>::const_iterator it = aoids.begin(); it != aoids.end(); ++it)
        {
            if (it != aoids.begin())
            {
                aoid_array << ",";
            }
            aoid_array << *it;
        }

        std::string sql = 
            "SELECT "
            "    A.name, "
            "    I.ql, "
            "    T.charname, "
            "    I.parent, "
            //"    CASE "
            //"        WHEN parent = 1 THEN 'Bank' "
            //"        WHEN parent = 2 THEN 'Inventory' "
            //"        WHEN parent = 3 THEN 'Shop' "
            //"        WHEN parent > 10 THEN 'Backpack' "
            //"        ELSE 'Unknown' "
            //"    END AS location "
            "    CASE "
            "        WHEN parent = 1 THEN 'Bank' "
            "        WHEN parent = 2 THEN 'Inventory' "
            "        WHEN parent = 3 THEN 'Shop' "
            "        WHEN I.parent > 10 THEN (SELECT "
            "            CASE "
            "                WHEN I2.parent = 1 THEN 'Bank' "
            "                WHEN I2.parent = 2 THEN 'Inventory' "
            "                WHEN I2.parent = 3 THEN 'Shop' "
            "            END "
            "            FROM tItems I2 WHERE I2.children = I.parent) "
            "    END AS container, "
            "    T.charid "
            "FROM "
            "    tItems I JOIN aodb.tblAO A ON I.keyhigh = A.aoid JOIN tToons T ON I.owner = T.charid "
            "WHERE keyhigh IN ";

        sql += "(" + aoid_array.str() + ")";

        m_result = g_DBManager.ExecTable(from_ascii_copy(sql));
    }


    ItemListDataModel::~ItemListDataModel()
    {
    }


    unsigned int ItemListDataModel::getColumnCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return COL_COUNT;
    }


    std::tstring ItemListDataModel::getColumnName(unsigned int index) const
    {
        if (!m_result)
        {
            return _T("");
        }

        switch (index)
        {
        case COL_ITEM_NAME:
            return _T("Item Name");
        case COL_ITEM_QL:
            return _T("QL");
        case COL_TOON_NAME:
            return _T("Character");
        case COL_BACKPACK_NAME:
            return _T("Backpack");
        case COL_BACKPACK_LOCATION:
            return _T("Location");
        default:
            return _T("");
        }
    }


    unsigned int ItemListDataModel::getItemCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return m_result->Rows();
    }


    std::tstring ItemListDataModel::getItemProperty(unsigned int index, unsigned int column) const
    {
        if (!m_result)
        {
            return _T("");
        }

        if (column == COL_BACKPACK_NAME)
        {
            try
            {
                unsigned int id = boost::lexical_cast<unsigned int>(m_result->Data(index, COL_BACKPACK_NAME));
                if (id <= 10)
                {
                    // If the item is in the bank or inventory we just display an empty name for the backpack.
                    return _T("");
                }
                unsigned int charid = boost::lexical_cast<unsigned int>(m_result->Data(index, COL_COUNT));
                return ServicesSingleton::Instance()->GetContainerName(charid, id);
            }
            catch (boost::bad_lexical_cast &/*e*/)
            {
                return _T("");
            }
        }

        return from_ascii_copy(m_result->Data(index, column));
    }


    unsigned int ItemListDataModel::getItemId(unsigned int index) const
    {
        if (!m_result)
        {
            return 0;
        }

        // The AOID is selected as column index 2 in the query
        return boost::lexical_cast<unsigned int>(m_result->Data(index, 2));
    }

}
