#include "stdafx.h"
#include "DataGridControl.h"
#include <boost/bind.hpp>

namespace aoia {

    DataGridControl::DataGridControl()
    {
        AtlInitCommonControls(ICC_LISTVIEW_CLASSES);
    }


    DataGridControl::~DataGridControl()
    {
    }


    void DataGridControl::setModel(DataGridModelPtr model)
    {
        // Clean out any previous data
        while (m_listView.GetHeader().GetItemCount())
        {
            m_listView.DeleteColumn(0);
        }
        m_listView.DeleteAllItems();

        if (m_model)
        {
            m_model->disconnect(m_addSignalConnection);
            m_model->disconnect(m_removeSignalConnection);
            m_model->disconnect(m_clearSignalConnection);
        }
        m_model = model;
        if (m_model)
        {
            m_addSignalConnection = m_model->connectItemAdded(boost::bind(&DataGridControl::onItemAdded, this, _1));
            m_removeSignalConnection = m_model->connectItemRemoved(boost::bind(&DataGridControl::onItemRemoved, this, _1));
            m_clearSignalConnection = m_model->connectAllRemoved(boost::bind(&DataGridControl::onAllItemsRemoved, this));
        }

        updateColumns();
        for (unsigned int i = 0; i < m_model->getItemCount(); ++i) {
            m_listView.InsertItem(i, LPSTR_TEXTCALLBACK);
        }
        autosizeColumns();
    }


    LRESULT DataGridControl::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        DefWindowProc(uMsg, wParam, lParam);
        m_listView = m_hWnd;
        m_listView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
        return 0;
    }


    LRESULT DataGridControl::onGetDispInfo(int wParam, LPNMHDR lParam, BOOL &bHandled)
    {
        NMLVDISPINFO *pdi = (NMLVDISPINFO*)lParam;
        if (pdi->item.mask & LVIF_TEXT) {
            std::tstring text = m_model->getItemProperty(pdi->item.iItem, pdi->item.iSubItem);
            ZeroMemory(pdi->item.pszText, pdi->item.cchTextMax);
            unsigned int len = text.length();
#ifdef UNICODE
            len = len * 2;
#endif
            memcpy(pdi->item.pszText, text.c_str(), len);
        }
        return 0;
    }


    void DataGridControl::onItemAdded(unsigned int index)
    {
        if (m_listView.GetItemCount() == 0) {
            updateColumns();
        }
        m_listView.InsertItem(index, LPSTR_TEXTCALLBACK);
        autosizeColumns();
    }


    void DataGridControl::onItemRemoved(unsigned int index)
    {
        m_listView.DeleteItem(index);
        if (m_listView.GetItemCount() == 0) {
            updateColumns();
        }
    }


    void DataGridControl::onAllItemsRemoved()
    {
        m_listView.DeleteAllItems();
        updateColumns();
    }


    void DataGridControl::updateColumns()
    {
        while (m_listView.GetHeader().GetItemCount()) {
            m_listView.DeleteColumn(0);
        }
        for (unsigned int i = 0; i < m_model->getColumnCount(); ++i) {
            m_listView.InsertColumn(i, m_model->getColumnName(i).c_str());
        }
    }


    void DataGridControl::autosizeColumns()
    {
        // MSDN: LVSCW_AUTOSIZE does not work as expected for column 0 for versions prior to comctl32.dll 5.80
        // Tested with later versions, still a problem in v 5.82
        /*int numCol = m_listView.GetHeader().GetItemCount();
        for (int i = 0; i < numCol; ++i) {
            m_listView.SetColumnWidth(i, LVSCW_AUTOSIZE);            
        }*/
        
        // Manually update
        unsigned int numRows = m_listView.GetItemCount();        
        TCHAR buffer[200];
        int numCol = m_listView.GetHeader().GetItemCount();
        for (int i = 0; i < numCol; ++i) 
        {            
            unsigned int maxWidth = m_listView.GetStringWidth(m_model->getColumnName(i).c_str());
            for(unsigned int rowIndex = 0; rowIndex < numRows; rowIndex++)
            {
                m_listView.GetItemText(rowIndex, i, buffer, 200);
                maxWidth = max(maxWidth, (unsigned int)m_listView.GetStringWidth(buffer));
            }
            m_listView.SetColumnWidth(i, max(maxWidth + 15, (unsigned int)m_listView.GetColumnWidth(i)));       
        }        
    }

}   // namespace
