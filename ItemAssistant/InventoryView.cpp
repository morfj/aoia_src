#include "stdafx.h"
#include "DBManager.h"
#include "InventoryView.h"
#include "AOMessageParsers.h"
#include "resource.h"
#include "shared/AODB.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#define TIXML_USE_STL
#include <TinyXml/tinyxml.h>

#include <map>


enum ColumnID
{
    COL_NAME,
    COL_QL,
    //COL_KEYLOW,
    //COL_KEYHIGH,
    COL_STACK,
    COL_CHARACTER,
    //COL_SLOT,
    //COL_VALUE,
    COL_CONTAINER,

    // this should be last always
    COL_COUNT
};


InventoryView::InventoryView()
:  m_sortDesc(false)
,  m_sortColumn(0)
{
}


InventoryView::~InventoryView()
{
}


LRESULT InventoryView::OnFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (!::IsWindowVisible(m_findview))
    {
        m_findview.ShowWindow(SW_SHOW);
    }
    m_findview.SetFocus();

    RECT rect;
    GetClientRect(&rect);
    CSize newsize(rect.right, rect.bottom);
    UpdateLayout(newsize);

    return 0;
}


LRESULT InventoryView::OnFindHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    HideFindWindow();
    return 0;
}


LRESULT InventoryView::OnInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (!::IsWindowVisible(m_infoview))
    {
        m_infoview.ShowWindow(SW_SHOW);
    }
    else
    {
        m_infoview.ShowWindow(SW_HIDE);
    }

    RECT rect;
    GetClientRect(&rect);
    CSize newsize(rect.right, rect.bottom);
    UpdateLayout(newsize);

    return 0;
}


LRESULT InventoryView::OnColumnClick(LPNMHDR lParam)
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


LRESULT InventoryView::OnItemActivate(LPNMHDR lParam)
{
    LPNMITEMACTIVATE param = (LPNMITEMACTIVATE)lParam;

    int sel = param->iItem;
    if (sel >= 0)
    {
        DWORD_PTR data = m_listview.GetItemData(sel);
        m_infoview.SetCurrentItem( (unsigned int) data );
    }

    return 0;
}


LRESULT InventoryView::OnItemContextMenu(LPNMHDR lParam)
{
    LPNMITEMACTIVATE param = (LPNMITEMACTIVATE)lParam;

    int sel = param->iItem;
    if (sel >= 0) {
        DWORD_PTR data = m_listview.GetItemData(sel);

        WTL::CPoint pos;
        GetCursorPos(&pos);

        WTL::CMenu menu;
        menu.LoadMenu(IDR_ITEM_POPUP);
        WTL::CMenuHandle popup = menu.GetSubMenu(0);
        int cmd = popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pos.x, pos.y, m_hWnd);
        
        if (cmd == 0) {
            // Menu was canceled
            return 0;
        }

        std::tstringstream sql;
        sql << _T("SELECT tItems.keylow, tItems.keyhigh, tItems.ql, name, ")
            << _T("(SELECT tToons.charname FROM tToons WHERE tToons.charid = owner) AS Character, ")
            << _T("parent, owner FROM tItems JOIN tblAO ON keylow = aoid WHERE itemidx = ") << (int)data;

        g_DBManager.Lock();
        SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
        g_DBManager.UnLock();

        std::tstring itemloid, itemhiid, itemql, itemname, itemlocation;
        if (pT && pT->Rows() > 0) {
            itemloid = from_ascii_copy(pT->Data(0, 0));
            itemhiid = from_ascii_copy(pT->Data(0, 1));
            itemql = from_ascii_copy(pT->Data(0, 2));
            itemname = from_ascii_copy(pT->Data(0, 3));
            unsigned int owner = boost::lexical_cast<int>(pT->Data(0, 6));
            unsigned int containerid = boost::lexical_cast<int>(pT->Data(0, 5));
            itemlocation = from_ascii_copy(pT->Data(0, 4));
            itemlocation += _T(" -> ");
            itemlocation += ServicesSingleton::Instance()->GetContainerName(owner, containerid);
        }
        else {
            assert(false); // Something went wrong with the SQL query.
            return 0; 
        }

        TCHAR buffer[1024];
        if (cmd == ID_COPY_AUNO_ITEMREF || cmd == ID_COPY_AUNO_ITEMREF_VBB) {
            ATL::AtlLoadString(IDS_AUNO_ITEMREF_URL, buffer, 1024);
        }
        else if (cmd == ID_COPY_JAYDEE_ITEMREF || cmd == ID_COPY_JAYDEE_ITEMREF_VBB) {
            ATL::AtlLoadString(IDS_JAYDEE_ITEMREF_URL, buffer, 1024);
        }
        std::tstring url(buffer);

        if (!url.empty()) {
            boost::replace_all(url, _T("%lowid%"), itemloid);
            boost::replace_all(url, _T("%hiid%"), itemhiid);
            boost::replace_all(url, _T("%ql%"), itemql);
        }

        std::tstringstream clip;
        switch (cmd) {
        case ID_COPY_AUNO_ITEMREF:
        case ID_COPY_JAYDEE_ITEMREF:
            clip << _T("<a href=\"") << url << _T("\">") << itemname << _T("</a> [QL") << itemql << _T("] [") << itemlocation << _T("]");
            break;
        case ID_COPY_AUNO_ITEMREF_VBB:
        case ID_COPY_JAYDEE_ITEMREF_VBB:
            clip << _T("[url=") << url << _T("]") << itemname << _T("[/url] {QL") << itemql << _T("} {") << itemlocation << _T("}");
            break;
        case ID_COPY_AO_ITEMREF:
            clip << _T("<a href=\"text://<a href=itemref://") << itemloid << _T("/") << itemhiid << _T("/") 
                << itemql << _T(">") << itemname << _T("</a> [QL") << itemql << _T("] [") << itemlocation 
                << _T("]<p><p><center>Generated by <a href='chatcmd:///start http://ia.frellu.net'>AOIA</a></center>\">") 
                << itemname << _T("</a>");
            break;
        default:
            assert(false);  // You probably forgot to add a handler for a new command on the context menu.
        }

        std::tstring str = clip.str();
        if (!str.empty() && OpenClipboard() && EmptyClipboard()) {
            HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (str.length() * 2 + 2)); 
            if (hglbCopy == NULL) {
                CloseClipboard();
                return true;
            }

            // Lock the handle and copy the text to the buffer.
            TCHAR* lptstrCopy = (TCHAR*)GlobalLock(hglbCopy);
            ZeroMemory(lptstrCopy, str.length() * 2);
            memcpy(lptstrCopy, str.c_str(), str.length() * 2);
            GlobalUnlock(hglbCopy);

            // Place the handle on the clipboard.
#ifdef UNICODE
            SetClipboardData(CF_UNICODETEXT, hglbCopy);
#else
            SetClipboardData(CF_TEXT, hglbCopy);
#endif
            CloseClipboard();
        }
    }

    return 0;
}


void InventoryView::HideFindWindow()
{
    if (::IsWindowVisible(m_findview))
    {
        m_listview.SetFocus();
        m_findview.ShowWindow(SW_HIDE);
    }

    RECT rect;
    GetClientRect(&rect);
    CSize newsize(rect.right, rect.bottom);
    UpdateLayout(newsize);
}


LRESULT InventoryView::OnCreate(LPCREATESTRUCT createStruct)
{
    m_findview.Create(m_hWnd);
    //m_findview.ShowWindow(SW_SHOWNOACTIVATE);
    m_findview.SetParent(this);
    m_findview.SetDlgCtrlID(IDW_FINDVIEW);

    m_infoview.Create(m_hWnd);
    m_infoview.SetParent(this);
    m_infoview.SetDlgCtrlID(IDW_INFOVIEW);

    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    m_splitter.Create(m_hWnd, rcDefault, NULL, style);
    m_splitter.SetSplitterExtendedStyle(0);

    m_treeview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS, WS_EX_CLIENTEDGE);
    m_treeview.SetDlgCtrlID(IDW_TREEVIEW);

    m_listview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | LVS_REPORT, WS_EX_CLIENTEDGE);
    m_listview.SetDlgCtrlID(IDW_LISTVIEW);

    //m_treeview.SetUnicodeFormat();
    m_treeRoot.SetOwner(this);
    m_treeview.SetRootItem(&m_treeRoot);

    m_splitter.SetSplitterPanes(m_treeview, m_listview);
    m_splitter.SetActivePane(SPLIT_PANE_LEFT);

    m_accelerators.LoadAccelerators(IDR_INV_ACCEL);

    PostMessage(WM_POSTCREATE);

    return 0;
}


void InventoryView::UpdateLayout(CSize newSize)
{
    CRect r( CPoint( 0, 0 ), newSize );

    if (::IsWindowVisible(m_findview))
    {
        RECT fvRect;
        m_findview.GetWindowRect(&fvRect);
        int height = fvRect.bottom - fvRect.top;
        ::SetWindowPos(m_findview, 0, r.left, r.top, r.Width(), height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
        r.top += height;
    }

    if (::IsWindowVisible(m_infoview))
    {
        RECT fvRect;
        m_infoview.GetWindowRect(&fvRect);
        int width = fvRect.right - fvRect.left;
        ::SetWindowPos(m_infoview, 0, r.right - width, r.top, width, r.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
        r.right -= width;
    }

    m_splitter.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
}


LRESULT InventoryView::OnSize(UINT wParam, CSize newSize)
{
    UpdateLayout(newSize);
    return 0;
}


LRESULT InventoryView::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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


void InventoryView::OnAOMessage(AO::Header* pMsg)
{
    ATLASSERT(pMsg != NULL);
    Native::AOMessageHeader msg(pMsg);

    switch(msg.msgid())
    {
    case AO::MSG_BANK:
        {
            Native::AOBank bank((AO::Bank*)pMsg);
            g_DBManager.Lock();
            g_DBManager.Begin();
            {  // Remove old stuff from the bank. Every update is a complete update.
                std::tstringstream sql;
                sql << _T("DELETE FROM tItems WHERE owner = ") << msg.charid() << _T(" AND parent = 1");
                g_DBManager.Exec(sql.str());
            }
            for (unsigned int i = 0; i < bank.numitems(); i++)
            {
                Native::AOItem item = bank.item(i);
                g_DBManager.InsertItem(
                    item.itemid().Low(),
                    item.itemid().High(),
                    item.ql(),
                    item.stack(),
                    1,                           // 1 = bank container
                    item.index(),
                    item.containerid().High(),
                    msg.charid());
            }
            g_DBManager.Commit();
            g_DBManager.UnLock();
        }
        break;

    case AO::MSG_CONTAINER:
        {
            Native::AOContainer bp((AO::Header*)pMsg);
            g_DBManager.Lock();
            g_DBManager.Begin();
            {
                // Remove old contents from BP
                std::tstringstream sql;
                sql << _T("DELETE FROM tItems WHERE parent = ") << bp.containerid().High();
                g_DBManager.Exec(sql.str());
            }
            // Register BP contents
            for (unsigned int i = 0; i < bp.numitems(); i++)
            {
                Native::AOItem item = bp.item(i);
                g_DBManager.InsertItem(
                    item.itemid().Low(),
                    item.itemid().High(),
                    item.ql(),
                    item.stack(),
                    bp.containerid().High(),
                    item.index(),
                    0,
                    msg.charid());
            }
            g_DBManager.Commit();
            g_DBManager.UnLock();
            //AddToTreeView(msg.charid(), bp.containerid().High());
        }
        break;

    case AO::MSG_FULLSYNC:
        {
            Native::AOEquip equip((AO::Equip*)pMsg);
            g_DBManager.Lock();
            g_DBManager.Begin();
            {
                // Remove old contents from DB
                std::tstringstream sql;
                sql << _T("DELETE FROM tItems WHERE parent = 2 AND owner = ") << equip.charid();
                g_DBManager.Exec(sql.str());
            }
            // Register items
            for (unsigned int i = 0; i < equip.numitems(); i++)
            {
                Native::AOItem item = equip.item(i);
                g_DBManager.InsertItem(
                    item.itemid().Low(),
                    item.itemid().High(),
                    item.ql(),
                    item.stack(),
                    2,             // parent 2 = equip
                    item.index(),
                    item.containerid().High(),
                    msg.charid());
            }
            g_DBManager.Commit();
            g_DBManager.UnLock();

            CleanupDB(equip.charid());
        }
        break;

    case AO::MSG_MOB_SYNC:
        {
            // Make sure this is the message for the currently playing toon (and not other mobs in vicinity).
            if (pMsg->charid == pMsg->target.high)
            {
                AO::MobInfo* pMobInfo = (AO::MobInfo*)pMsg;
                std::string name(&(pMobInfo->characterName.str), pMobInfo->characterName.strLen - 1);

                Native::AOMessageHeader header(pMsg);

                g_DBManager.Lock();
                g_DBManager.SetToonName(header.charid(), from_ascii_copy(name));
                g_DBManager.UnLock();
            }
        }
        break;

    }
}


void InventoryView::CleanupDB(unsigned int charid)
{
    // Statement to delete all the orfaned containers for this toon.
    std::tstringstream sql;
    sql << _T("DELETE FROM tItems WHERE owner = ") << charid << _T(" AND parent > 2 AND parent NOT IN (SELECT DISTINCT children FROM tItems)");

    g_DBManager.Lock();
    g_DBManager.Exec(sql.str());
    g_DBManager.UnLock();
}


bool InventoryView::PreTranslateMsg(MSG* pMsg)
{
    if (m_accelerators.TranslateAccelerator(m_hWnd, pMsg))
    {
        return true;
    }
    if (m_findview.PreTranslateMsg(pMsg))
    {
        return true;
    }

    return false;
}


// Updates the list view with the results of the SQL query. 'where' is used as the expression after the WHERE keyword.
void InventoryView::UpdateListView(std::tstring const& where)
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
int InventoryView::CompareStr(LPARAM param1, LPARAM param2, LPARAM sort)
{
    InventoryView* pThis = (InventoryView*)sort;

    int result = 0;

    TCHAR name1[MAX_PATH];
    TCHAR name2[MAX_PATH];

    ZeroMemory(name1, sizeof(name1));
    ZeroMemory(name2, sizeof(name2));

    pThis->m_listview.GetItemText((int)param1, pThis->m_sortColumn, name1, sizeof(name1) - 1);
    pThis->m_listview.GetItemText((int)param2, pThis->m_sortColumn, name2, sizeof(name2) - 1);

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

    return result;
}







InfoView::InfoView()
: m_pParent(NULL) 
{
}


void InfoView::SetParent(InventoryView* parent)
{
    m_pParent = parent;
}


BOOL InfoView::PreTranslateMsg(MSG* pMsg)
{
    return IsDialogMessage(pMsg);
}


LRESULT InfoView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    this->SetWindowText(_T("Info View"));

    DlgResize_Init(false, true, WS_CLIPCHILDREN);
    return 0;
}


LRESULT InfoView::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    LPMSG pMsg = (LPMSG)lParam;
    return this->PreTranslateMsg(pMsg);
}


void InfoView::SetCurrentItem(unsigned int item)
{
    m_currentItem = item;

    WTL::CListViewCtrl lv(GetDlgItem(IDC_LISTVIEW));

    lv.DeleteAllItems();

    std::tstringstream sql;
    sql << _T("SELECT * FROM tItems WHERE itemidx = ") << item;

    g_DBManager.Lock();
    SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
    g_DBManager.UnLock();

    if (pT)
    {
        if (pT->Rows() > 0)
        {
            // Backup previous column widths
            std::vector<int> columnWidthList;
            while(lv.GetHeader().GetItemCount() > 0)
            {
                columnWidthList.push_back(lv.GetColumnWidth(0));
                lv.DeleteColumn(0);
            }

            // Rebuild columns with new labels
            lv.AddColumn(_T("Property"), 0);
            lv.AddColumn(_T("Value"), 1);

            int width = columnWidthList.size() > 0 ? columnWidthList[0] : 75;
            lv.SetColumnWidth(0, width);
            width = columnWidthList.size() > 1 ? columnWidthList[1] : 120;
            lv.SetColumnWidth(1, width);

            // Fill content
            int indx = INT_MAX - 1;
            for (unsigned int col = 0; col < pT->Columns(); col++)
            {
                indx = lv.AddItem(indx, 0, from_ascii_copy(pT->Headers()[col]).c_str());
                lv.AddItem(indx, 1, from_ascii_copy(pT->Data()[col]).c_str());
            }
        }
    }
}







void FindView::SetParent(InventoryView* parent)
{
    m_pParent = parent;
}


LRESULT FindView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    this->SetWindowText(_T("Find View"));

    DlgResize_Init(false, true, WS_CLIPCHILDREN);
    return 0;
}


LRESULT FindView::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    LPMSG pMsg = (LPMSG)lParam;
    return this->PreTranslateMsg(pMsg);
}


BOOL FindView::PreTranslateMsg(MSG* pMsg)
{
    return IsDialogMessage(pMsg);
}


LRESULT FindView::OnCbnBuildCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    KillTimer(1);

    CComboBox cb = GetDlgItem(IDC_CHARCOMBO);

    int oldselection = cb.GetCurSel();

    cb.ResetContent();
    int item = cb.AddString(_T("-"));
    cb.SetItemData(item, 0);

    g_DBManager.Lock();
    SQLite::TablePtr pT = g_DBManager.ExecTable(_T("SELECT DISTINCT owner FROM tItems"));
    g_DBManager.UnLock();

    if (pT != NULL)
    {
        for (unsigned int i = 0; i < pT->Rows(); i++)
        {
            unsigned int id = atoi(pT->Data()[pT->Columns()*i].c_str());

            g_DBManager.Lock();
            std::tstring name = g_DBManager.GetToonName(id);
            g_DBManager.UnLock();

            if (name.empty())
            {
                name = from_ascii_copy(pT->Data()[pT->Columns()*i]);
            }

            if ((item = cb.AddString(name.c_str())) != CB_ERR)
            {
                cb.SetItemData(item, id);
            }
        }
    }

    if (oldselection >= 0)
    {
        cb.SetCurSel(oldselection);
    }

    return 0;
}


LRESULT FindView::OnEnChangeItemtext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    SetTimer(1, 1500);
    return 0;
}


LRESULT FindView::OnCbnSelChangeCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    UpdateFindQuery();
    return 0;
}


LRESULT FindView::OnTimer(UINT wParam, TIMERPROC lParam)
{
    if (wParam == 1)
    {
        UpdateFindQuery();
        KillTimer(1);
    }
    return 0;
}


void FindView::UpdateFindQuery()
{
    KillTimer(1);

    CComboBox cb = GetDlgItem(IDC_CHARCOMBO);
    CEdit eb = GetDlgItem(IDC_ITEMTEXT);

    unsigned int charid = 0;
    int item = -1;
    if ((item = cb.GetCurSel()) != CB_ERR)
    {
        charid = (unsigned int)cb.GetItemData(item);
    }

    TCHAR buffer[MAX_PATH];
    ZeroMemory(buffer, MAX_PATH);
    eb.GetWindowText(buffer, MAX_PATH);
    std::tstring text(buffer);

    if ((text.size() > 2) && ((m_lastQueryText != text) || (m_lastQueryChar != charid)))
    {
        m_lastQueryText = text;
        m_lastQueryChar = charid;
        std::tstringstream sql;

        if (charid > 0)
        {
            sql << _T("owner = ") << charid << _T(" AND ");
        }
        sql << _T("keylow IN (SELECT aoid FROM aodb.tblAO WHERE name LIKE \"%") << text << _T("%\")");

        m_pParent->UpdateListView(sql.str());
    }
}
