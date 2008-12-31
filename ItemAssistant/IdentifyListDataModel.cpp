#include "StdAfx.h"
#include "IdentifyListDataModel.h"
#include "DBManager.h"


namespace aoia {

    using namespace SQLite;

    IdentifyListDataModel::IdentifyListDataModel()
    {
        m_result = g_DBManager.ExecTable(_T("SELECT AO.name, I.purpose, I.aoid FROM tblIdentify I JOIN tblAO AO ON I.highid = AO.aoid ORDER BY AO.name"));
    }


    IdentifyListDataModel::~IdentifyListDataModel()
    {
    }


    unsigned int IdentifyListDataModel::getColumnCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return 2;
    }


    std::tstring IdentifyListDataModel::getColumnName(unsigned int index) const
    {
        if (!m_result)
        {
            return _T("");
        }

        if (index == 0)
        {
            return _T("Item");
        }
        if (index == 1)
        {
            return _T("Purpose");
        }
        return _T("");
    }


    unsigned int IdentifyListDataModel::getItemCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return m_result->Rows();
    }


    std::tstring IdentifyListDataModel::getItemProperty(unsigned int index, unsigned int column) const
    {
        if (!m_result)
        {
            return _T("");
        }

        return from_ascii_copy(m_result->Data(index, column));
    }


    unsigned int IdentifyListDataModel::getItemId(unsigned int index) const
    {
        if (!m_result)
        {
            return 0;
        }

        // The AOID is selected as column index 2 in the query
        return boost::lexical_cast<unsigned int>(m_result->Data(index, 2));
    }

}
