#include "StdAfx.h"
#include "IdentifyView.h"
#include <Shared/SQLite.h>
#include "DBManager.h"
#include "QueryDataGridModel.h"
#include "IdentifyListDataModel.h"


using namespace SQLite;
using namespace aoia;


IdentifyView::IdentifyView()
    : m_datagrid(new DataGridControl())
    , m_identifyList(new DataGridControl())
{
}


IdentifyView::~IdentifyView()
{
}


LRESULT IdentifyView::onCreate(LPCREATESTRUCT createStruct)
{
    RECT identRect = { 0, 0, 250, createStruct->cy };
    RECT gridRect = { 250, 0, createStruct->cx, createStruct->cy };

    // Create child windows
    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
    m_identifyList->Create(m_hWnd, identRect, NULL, style, WS_EX_CLIENTEDGE);
    m_identifyList->SetDlgCtrlID(IDC_IDENTLIST);

    m_identifyListModel.reset(new IdentifyListDataModel());
    m_identifyList->setModel(m_identifyListModel);

    m_datagrid->Create(m_hWnd, gridRect, NULL, style, WS_EX_CLIENTEDGE);
    m_datagrid->SetDlgCtrlID(IDC_DATAGRID);

    DlgResize_Init(false);

    return 0;
}


LRESULT IdentifyView::onListItemChanging(LPNMHDR lParam)
{
    // Check that it is an event from the correct child window.
    if (lParam->hwndFrom == m_identifyList->m_hWnd)
    {
        LPNMLISTVIEW pItem = (LPNMLISTVIEW)lParam;

        // Check to see if the change is a selection event.
        if ( !(pItem->uOldState & LVIS_SELECTED) && (pItem->uNewState & LVIS_SELECTED) )
        {
            unsigned int aoid = m_identifyListModel->getItemId(pItem->iItem);

            std::tstringstream sql;
            sql << _T("SELECT *, ")
                << _T("(SELECT CASE ")
                << _T("     WHEN parent = 0 THEN 'Unknown' ")
                << _T("     WHEN parent = 1 THEN 'Bank' ")
                << _T("     WHEN parent = 2 THEN 'Inventory' ")
                << _T("     WHEN parent = 3 THEN 'Shop' ")
                << _T("     ELSE 'Backpack' ")
                << _T(" END) AS Location ")
                << _T("FROM tItems I JOIN aodb.tblAO A ON I.keyhigh = A.aoid JOIN tToons T ON I.owner = T.charid ")
                << _T("WHERE keyhigh = ") << aoid;

            boost::shared_ptr<QueryDataGridModel> data(new QueryDataGridModel(sql.str()));
            m_datagrid->setModel(data);
        }
    }

    return FALSE;
}
