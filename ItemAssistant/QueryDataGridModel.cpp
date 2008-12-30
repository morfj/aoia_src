#include "StdAfx.h"
#include "QueryDataGridModel.h"
#include "DBManager.h"


namespace aoia {

    using namespace SQLite;

    QueryDataGridModel::QueryDataGridModel(std::tstring const& sql)
    {
        m_result = g_DBManager.ExecTable(sql);
    }


    QueryDataGridModel::~QueryDataGridModel()
    {
    }


    unsigned int QueryDataGridModel::getColumnCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return m_result->Columns();
    }


    std::tstring QueryDataGridModel::getColumnName(unsigned int index) const
    {
        if (!m_result)
        {
            return _T("");
        }

        return from_ascii_copy(m_result->Headers().at(index));
    }


    unsigned int QueryDataGridModel::getItemCount() const
    {
        if (!m_result)
        {
            return 0;
        }

        return m_result->Rows();
    }


    std::tstring QueryDataGridModel::getItemProperty(unsigned int index, unsigned int column) const
    {
        if (!m_result)
        {
            return _T("");
        }

        return from_ascii_copy(m_result->Data(index, column));
    }

}
