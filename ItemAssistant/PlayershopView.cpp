#include "StdAfx.h"
#include "PlayershopView.h"
//#include <boost/filesystem/path.hpp>
//#include <boost/filesystem/fstream.hpp>
//#include <boost/filesystem/operations.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/find_iterator.hpp>
//#include <boost/algorithm/string/find_format.hpp>
//#include <boost/algorithm/string/replace.hpp>
//#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/find.hpp>

enum ColumnID
{
    TEXT,
    // this should be last always
    COL_COUNT
};

PlayershopView::PlayershopView(void)
: m_sortColumn(0)
{
   m_directoryWatch.Begin();
}

PlayershopView::~PlayershopView(void)
{
   m_directoryWatch.StopPlease();
   m_directoryWatch.End();
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
   int colid = m_listview.AddColumn(_T("Info"), 0);
   m_listview.SetColumnWidth(colid, 550);

   colid = m_listview.AddColumn(_T("Toon"), 1);
   m_listview.SetColumnWidth(colid, 100);

   m_treeRoot.SetOwner(this);
   m_treeview.SetRootItem(&m_treeRoot);
   
   m_splitter.SetSplitterPanes(m_treeview, m_listview);
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
//	m_splitter.SetSplitterPos(200);

	return 0;
}

// Updates the list view with the results of the SQL query. 'where' is used as the expression after the WHERE keyword.
void PlayershopView::UpdateListView(std::tstring const& where)
{
   m_listview.DeleteAllItems();
   m_listview.AddItem(0,0,where.c_str());
}

// Updates the list view with the results of the SQL query. 'where' is used as the expression after the WHERE keyword.
void PlayershopView::UpdateListView(std::vector<std::tstring> v)
{
   m_listview.DeleteAllItems();

   for(unsigned int i=0; i<v.size();i++){
      m_listview.AddItem(0,i%2,v[i].c_str());
   }
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


void PlayershopView::OnActive(bool doActivation)
{
    if (doActivation && m_splitter.GetSplitterPos() < 0) {
        m_splitter.SetSplitterPos(200);
    }

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


}


void WatchDirectoryThread::WatchDirectory(LPTSTR lpDir)
{
   // Create a HANDLE to the directory we want to watch
   HANDLE directory = CreateFile(
      lpDir,                          
      GENERIC_READ,
      FILE_SHARE_READ|FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS ,
      NULL
   );

   // Create a buffer to receive the changed filenames
   char buffer[4096];

   while (!m_term) 
   { 

      // Wait for notification
      DWORD bytesReturned = 0;

      ReadDirectoryChangesW(
         directory,
         buffer,
         4096,
         TRUE,
         FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_CREATION,
         &bytesReturned,
         NULL,
         NULL
      );

      std::tstringstream f;
      f << lpDir << "\\";

      // Did not find any information on the data returned,
      // so the filename is retreived based on trial and error
      for(int i=12;i<bytesReturned;i=i+2)
      {  
         f << buffer[i];
      }

      std::tstring filename(f.str());
      const wchar_t* ptr = filename.c_str();

      // Proceed only if the directory change notification is triggered by
      // the file PlayerShopLog.html

      if(boost::find_last(ptr, "PlayerShopLog.html"))
      {
         boost::filesystem::path p(to_utf8_copy(filename),boost::filesystem::native);

         std::ifstream in(p.string().c_str());
         std::string line;
         std::string text;
         std::vector<std::tstring> v;
         while(in){
            line.clear();
            std::getline(in,line);
            if(!line.empty())
            {
               text += line;
            }
         }

         // Now that we have the whole file, lets parse it

         // Text located between the two following tags is to be considered an item sold
         std::string startTag = "<div indent=wrapped>" ;
         std::string endTag   = "</div>" ;
         std::stringstream popupText;
         while(text.length() > 0)
         {
         
            std::string::size_type start = text.find( startTag, 0 );
            std::string::size_type end  = text.find( endTag , 0 );
            if( start != std::string::npos && end != std::string::npos)
            {
               // adding text for ballon message
               popupText << text.substr(start+startTag.length(),end-start-startTag.length()) << "\r\n";

               // remove the already processed part of the string
               text = text.substr(end+endTag.length());
            }
            else
            {
               text = "";
            }
         }

         if(!popupText.str().empty())
         {
            ServicesSingleton::Instance()->ShowTrayIconBalloon(STREAM2STR(popupText.str().c_str()));
         }
      }
   }
}

void WatchDirectoryThread::RefreshDirectory(LPTSTR lpDir)
{
   // This is where you might place code to refresh your
   // directory listing, but not the subtree because it
   // would not be necessary.

   _tprintf(TEXT("Directory (%s) changed.\n"), lpDir);
}

void WatchDirectoryThread::RefreshTree(LPTSTR lpDrive)
{
   // This is where you might place code to refresh your
   // directory listing, including the subtree.

   _tprintf(TEXT("Directory tree (%s) changed.\n"), lpDrive);
}

/*************************************************/
/** Worker Thread                               **/
/*************************************************/

DWORD WatchDirectoryThread::ThreadProc()
{
   m_term = false;
   std::tstring filename;
   filename = STREAM2STR( g_DBManager.AOFolder() << _T("\\Prefs") );

   WatchDirectory((LPTSTR)filename.c_str());


   m_term = false;

   return 0;
}