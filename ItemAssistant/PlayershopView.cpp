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
   //DWORD dwWaitStatus; 
/*   HANDLE dwChangeHandles[2]; 
   TCHAR lpDrive[4];
   TCHAR lpFile[_MAX_FNAME];
   TCHAR lpExt[_MAX_EXT];

   _tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

   lpDrive[2] = (TCHAR)'\\';
   lpDrive[3] = (TCHAR)'\0';
 
// Watch the directory for file creation and deletion.  
   dwChangeHandles[0] = FindFirstChangeNotification( 
      lpDir,                          // directory to watch 
      TRUE,                          // watch the subtree 
      FILE_NOTIFY_CHANGE_LAST_WRITE |
	  FILE_NOTIFY_CHANGE_CREATION);  // watch file write or creation
 */
HANDLE directory = CreateFile(
      lpDir,                          // directory to watch 
      GENERIC_READ,
      FILE_SHARE_READ|FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS ,
      NULL
);

char buffer[4096];
/*

BOOL WINAPI ReadDirectoryChangesW(
  directory,
  buffer,
  4096,
  TRUE,
  FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_CREATION
);
*/
/*
   if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
   {
     printf("\n ERROR: FindFirstChangeNotification function failed.\n");
     ExitProcess(GetLastError()); 
   }
 */
// Watch the subtree for directory creation and deletion.  
/*   dwChangeHandles[1] = FindFirstChangeNotification( 
      lpDrive,                       // directory to watch 
      TRUE,                          // watch the subtree 
      FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 
 
   if (dwChangeHandles[1] == INVALID_HANDLE_VALUE) 
   {
     printf("\n ERROR: FindFirstChangeNotification function failed.\n");
     ExitProcess(GetLastError()); 
   }
 */

// Make a final validation check on our handles.
/*   if ((dwChangeHandles[0] == NULL))// || (dwChangeHandles[1] == NULL))
   {
     printf("\n ERROR: Unexpected NULL from FindFirstChangeNotification.\n");
     ExitProcess(GetLastError()); 
   }
*/
// Change notification is set. Now wait on both notification 
// handles and refresh accordingly.  
   while (TRUE) 
   { 
      // Wait for notification. 
      printf("\nWaiting for notification...\n");
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

//      RefreshTree(NULL); 

      std::tstringstream f;
      f << lpDir << "\\";
      for(int i=12;i<bytesReturned;i=i+2)
      {
      f << buffer[i];
      }

   std::tstring filename(f.str());

    //char text[]="hello dolly!";
   //iterator_range<char*> result=find_last(filename.c_str(),"PlayerShopLog.html");

    //transform( result.begin(), result.end(), result.begin(), bind2nd(plus<char>(), 1) );
    // text = "hello dommy!"            

    //to_upper(result); // text == "hello doMMy!"

    // iterator_range is convertible to bool
   const wchar_t* ptr = filename.c_str();
   if(!boost::find_last(ptr, "PlayerShopLog.html"))
    {
        return;
    }

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
         // adding data for column 1
         //v.push_back(from_ascii_copy(text.substr(start+startTag.length(),end-start-startTag.length())));
         popupText << text.substr(start+startTag.length(),end-start-startTag.length()) << "\r\n";

         // remove the already processed part of the string
         text = text.substr(end+endTag.length());
      }
      else
      {
         text = "";
      }
   }

/*
   if(!m_popup)
   {
      m_popup = new CPlayerShopPopupDlg;
   }
   
   if(!::IsWindow(m_popup->GetSafeHwnd()))
   {
      m_popup->Create(IDD_PLAYERSHOPPOPUP,this);
   }
*/
   ServicesSingleton::Instance()->ShowTrayIconBalloon(STREAM2STR(popupText.str().c_str()));
//   popup.Create(NULL);
/*
   PlayerShopPopupDlg popup;
   popup.Create(NULL);
   popup.ShowWindow(SW_SHOW);
   popup->SetText(popupText.str());
*/
   //popup.ShowWindow(0);
   //popup.DoModal();
//   m_popup->SetText(popupText.str());
//   m_popup->ShowWindow(SW_SHOW);

   //Sleep(5000);
   //popup.ShowWindow(SW_HIDE);
   //popup.DestroyWindow();

/*

      dwWaitStatus = WaitForMultipleObjects(1, dwChangeHandles, 
         FALSE, INFINITE); 
 
      switch (dwWaitStatus) 
      { 
         case WAIT_OBJECT_0: 
 
         // A file was created or written.
         // Refresh tree
             RefreshTree(NULL); 
             if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
             {
               printf("\n ERROR: FindNextChangeNotification function failed.\n");
               ExitProcess(GetLastError()); 
             }
             break; 
 
         case WAIT_OBJECT_0 + 1: 
 
         // A directory was created, renamed, or deleted.
         // Refresh the tree and restart the notification. 
             RefreshTree(lpDrive); 
             if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
             {
               printf("\n ERROR: FindNextChangeNotification function failed.\n");
               ExitProcess(GetLastError()); 
             }
             break; 
 
         case WAIT_TIMEOUT:

         // A time-out occurred. This would happen if some value other 
         // than INFINITE is used in the Wait call and no changes occur.
         // In a single-threaded environment, you might not want an
         // INFINITE wait.
 
            printf("\nNo changes in the time-out period.\n");
            break;

         default: 
            printf("\n ERROR: Unhandled dwWaitStatus.\n");
            ExitProcess(GetLastError());
            break;
      }*/
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

   do
   {
      WatchDirectory((LPTSTR)filename.c_str());
//      WatchDirectory(dummy.c_str());
   } 
   while (!m_term);

   m_term = false;

   return 0;
}