#include "StdAfx.h"
#include "PlayershopView.h"


enum ColumnID
{
    TEXT,
    // this should be last always
    COL_COUNT
};

PlayershopView::PlayershopView(void)
: m_sortColumn(0)
{
}

PlayershopView::~PlayershopView(void)
{
}


LRESULT PlayershopView::OnColumnClick(LPNMHDR lParam)
{
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW) lParam;

    if (m_sortColumn != pnmv->iSubItem)
    {
        m_sortColumn = pnmv->iSubItem;
        m_sortDesc = false;
    }
    else
    {
        m_sortDesc = !m_sortDesc;
    }

    m_listview.SortItemsEx(CompareStr, (LPARAM)this);

    return 0;
}

LRESULT PlayershopView::OnItemActivate(LPNMHDR lParam)
{
    LPNMITEMACTIVATE param = (LPNMITEMACTIVATE)lParam;

    int sel = param->iItem;
    if (sel >= 0)
    {
        DWORD_PTR data = m_listview.GetItemData(sel);
    }

    return 0;
}

LRESULT PlayershopView::OnCreate(LPCREATESTRUCT createStruct)
{
   DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

   m_splitter.Create(m_hWnd, rcDefault, NULL, style);
   m_splitter.SetSplitterExtendedStyle(0);

   m_treeview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS, WS_EX_CLIENTEDGE);
   m_treeview.SetDlgCtrlID(IDW_PSMTREEVIEW);

   m_listview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | LVS_REPORT, WS_EX_CLIENTEDGE);
   m_listview.SetDlgCtrlID(IDW_PSMLISTVIEW);
   m_listview.AddColumn(_T("Text"), 0);

   m_treeRoot.SetOwner(this);
   m_treeview.SetRootItem(&m_treeRoot);
   
   m_splitter.SetSplitterPanes(m_treeview, m_listview);
//   m_splitter.SetSplitterPanes(m_treeview, NULL);
   m_splitter.SetActivePane(SPLIT_PANE_LEFT);

   PostMessage(WM_POSTCREATE);

   return 0;

}


void PlayershopView::UpdateLayout(CSize newSize)
{
   CRect r( CPoint( 0, 0 ), newSize );

   m_splitter.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
}


LRESULT PlayershopView::OnSize(UINT wParam, CSize newSize)
{
   UpdateLayout(newSize);
	return 0;
}

LRESULT PlayershopView::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//SetSplitterPos works best after the default WM_CREATE has been handled
	m_splitter.SetSplitterPos(200);

   CTreeItem item = m_treeview.GetRootItem();
   item = m_treeview.GetChildItem(item);
   if (!item.IsNull())
   {
      item = m_treeview.GetChildItem(item);
   }
   if (!item.IsNull())
   {
      m_treeview.SelectItem(item);
   }

	return 0;
}

// Updates the list view with the results of the SQL query. 'where' is used as the expression after the WHERE keyword.
void PlayershopView::UpdateListView(std::tstring const& where)
{
   m_listview.DeleteAllItems();

}

// Static callback function for sorting items.
int PlayershopView::CompareStr(LPARAM param1, LPARAM param2, LPARAM sort)
{
   PlayershopView* pThis = (PlayershopView*)sort;

   int result = 0;
/*
   TCHAR name1[MAX_PATH];
   TCHAR name2[MAX_PATH];

   ZeroMemory(name1, sizeof(name1));
   ZeroMemory(name2, sizeof(name2));

   pThis->m_listview.GetItemText(param1, pThis->m_sortColumn, name1, sizeof(name1) - 1);
   pThis->m_listview.GetItemText(param2, pThis->m_sortColumn, name2, sizeof(name2) - 1);

   switch (pThis->m_sortColumn)
   {
   case COL_QL:
   case COL_STACK:
      {
         int a = atoi(to_ascii_copy(name1).c_str());
         int b = atoi(to_ascii_copy(name2).c_str());
         result = pThis->m_sortDesc ? b - a : a - b;
      }
      break;
   default:
      result = pThis->m_sortDesc ? StrCmpI(name2, name1) : StrCmpI(name1, name2);
   }
*/
   return result;
}
