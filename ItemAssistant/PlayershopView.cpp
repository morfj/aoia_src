#include "StdAfx.h"
#include "PlayershopView.h"

PlayershopView::PlayershopView(void)
{
}

PlayershopView::~PlayershopView(void)
{
}

LRESULT PlayershopView::OnCreate(LPCREATESTRUCT createStruct)
{
   DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

   m_splitter.Create(m_hWnd, rcDefault, NULL, style);
	m_splitter.SetSplitterExtendedStyle(0);

   m_treeview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS, WS_EX_CLIENTEDGE);
   m_listview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | LVS_REPORT, WS_EX_CLIENTEDGE);

   m_treeRoot.SetOwner(this);
   
   std::tstring s(_T("All Characters"));
   m_treeRoot.SetLabel(s);
   std::vector<PsmTreeViewItem*> result = m_treeRoot.GetChildren();
   m_treeview.SetRootItem(&m_treeRoot);

//   m_splitter.SetSplitterPanes(m_treeview, m_listview);
   m_splitter.SetSplitterPanes(m_treeview, NULL);
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

   std::tstringstream sql;
   //sql << "SELECT substr(name,0,length(name)-1) AS Name, tItems.ql AS QL, stack AS Stack, owner AS Character, parent AS Container FROM tItems JOIN tblAO ON keylow = aoid";
   sql << _T("SELECT tItems.itemidx, owner, name AS Name, tItems.ql AS QL, stack AS Stack, ")
       << _T("(SELECT tToons.charname FROM tToons WHERE tToons.charid = owner) AS Character, ")
       << _T("parent AS Container FROM tItems JOIN tblAO ON keylow = aoid");

   if (!where.empty())
   {
      sql << _T(" WHERE ") << where;
   }

   g_DBManager.Lock();
   SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
   g_DBManager.UnLock();

   if (pT)
   {
      if (pT->Rows() > 0)
      {
         // Backup previous column widths and labels
         std::map<std::tstring, int> columnWidthMap;

         while(m_listview.GetHeader().GetItemCount() > 0)
         {
            // Get column label
            TCHAR buffer[1024];
            ZeroMemory(buffer, sizeof(buffer));
            LVCOLUMN columnInfo;
            columnInfo.mask = LVCF_TEXT;
            columnInfo.pszText = buffer;
            columnInfo.cchTextMax = sizeof(buffer);
            m_listview.GetColumn( 0, &columnInfo);

            // Store label an width
            columnWidthMap[buffer] = m_listview.GetColumnWidth(0);

            m_listview.DeleteColumn(0);
         }

         //// Rebuild columns with new labels
         //for (unsigned int col = 0; col < pT->Columns()-1; col++)
         //{
         //   m_listview.AddColumn(pT->Headers()[col+1].c_str(), col);
         //   int width = columnWidthList.size() > col ? columnWidthList[col] : 100;
         //   m_listview.SetColumnWidth(col, width);
         //}
         std::map<std::tstring, int> columnmap;

         int indx = INT_MAX - 1;
         for (unsigned int row = 0; row < pT->Rows(); row++)
         {
            for (unsigned int col = 2; col < pT->Columns(); col++)
            {
               std::tstring column = from_ascii_copy(pT->Headers()[col]);
               if (columnmap.find(column) == columnmap.end())
               {
                  columnmap[column] = m_listview.AddColumn(column.c_str(), col-2);
                  int w = columnWidthMap.find(column) != columnWidthMap.end() ? columnWidthMap[column] : 100;
                  m_listview.SetColumnWidth(columnmap[column], w);
               }

               std::tstring data = from_ascii_copy(pT->Data(row, col));

               // Find proper container name
               if (column == _T("Container"))
               {
                  unsigned int id = atoi(pT->Data(row, col).c_str());
                  unsigned int charid = atoi(pT->Data(row, 1).c_str());

                  data = ServicesSingleton::Instance()->GetContainerName(charid, id);
               }
               indx = m_listview.AddItem(indx, columnmap[column], data.c_str());
            }
            if (pT->Columns() > 0)
            {
               int data = atoi(pT->Data(row, 0).c_str());
               m_listview.SetItemData(indx, data);
            }
         }
      }
   }

   m_listview.SortItemsEx(CompareStr, (LPARAM)this);
}

// Static callback function for sorting items.
int PlayershopView::CompareStr(LPARAM param1, LPARAM param2, LPARAM sort)
{
   PlayershopView* pThis = (PlayershopView*)sort;

   int result = 0;

   TCHAR name1[MAX_PATH];
   TCHAR name2[MAX_PATH];

   ZeroMemory(name1, sizeof(name1));
   ZeroMemory(name2, sizeof(name2));

   pThis->m_listview.GetItemText(param1, pThis->m_sortColumn, name1, sizeof(name1) - 1);
   pThis->m_listview.GetItemText(param2, pThis->m_sortColumn, name2, sizeof(name2) - 1);
/*
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
