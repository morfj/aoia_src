#include "StdAfx.h"
#include "IdentifyView.h"
#include <Shared/SQLite.h>
#include "DBManager.h"
#include "QueryDataGridModel.h"


using namespace SQLite;
using namespace aoia;


IdentifyView::IdentifyView()
    : m_datagrid(new DataGridControl())
{
}


IdentifyView::~IdentifyView()
{
}


LRESULT IdentifyView::onCreate(LPCREATESTRUCT createStruct)
{
    RECT identRect = { 0, 0, 250, createStruct->cy };
    RECT gridRect = { 250, 0, createStruct->cx - 250, createStruct->cy };

    // Create child windows
    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
    m_identifyableList.Create(m_hWnd, identRect, NULL, style, WS_EX_CLIENTEDGE);
    m_identifyableList.SetDlgCtrlID(IDC_IDENTLIST);

    m_datagrid->Create(m_hWnd, gridRect, NULL, style, WS_EX_CLIENTEDGE);
    m_datagrid->SetDlgCtrlID(IDC_DATAGRID);

    DlgResize_Init(false);

    // Populate the list of items we can identify
    TablePtr pTable = g_DBManager.ExecTable(_T("SELECT I.aoid, AO.name, I.purpose FROM tblIdentify I JOIN tblAO AO ON I.highid = AO.aoid"));

    if (pTable)
    {
        m_identifyableList.AddColumn(_T("Item"), 0);
        m_identifyableList.AddColumn(_T("Purpose"), 1);

        int index = INT_MAX - 1;
        for (unsigned int i = 0; i < pTable->Rows(); ++i)
        {
            try 
            {
                unsigned int aoid = boost::lexical_cast<unsigned int>(pTable->Data(i, 0));
                std::string name = pTable->Data(i, 1);
                std::string purpose = pTable->Data(i, 2);

                index = m_identifyableList.AddItem(index, 0, from_ascii_copy(name).c_str());
                index = m_identifyableList.AddItem(index, 1, from_ascii_copy(purpose).c_str());
                m_identifyableList.SetItemData(index, aoid);
            }
            catch (boost::bad_lexical_cast &/*e*/)
            {
                // Skip this row
            }
        }
    }

    return 0;
}


LRESULT IdentifyView::onListItemChanging(LPNMHDR lParam)
{
    // Check that it is an event from the correct child window.
    if (lParam->hwndFrom == m_identifyableList.m_hWnd)
    {
        LPNMLISTVIEW pItem = (LPNMLISTVIEW)lParam;

        // Check to see if the change is a selection event.
        if ( !(pItem->uOldState & LVIS_SELECTED) && (pItem->uNewState & LVIS_SELECTED) )
        {
            unsigned int aoid = m_identifyableList.GetItemData(pItem->iItem);

            std::tstringstream sql;
            sql << _T("SELECT * FROM tItems WHERE keyhigh = ") << aoid;

            boost::shared_ptr<QueryDataGridModel> data(new QueryDataGridModel(sql.str()));
            m_datagrid->setModel(data);
        }
    }

    return FALSE;
}
