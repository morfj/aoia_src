#include "stdafx.h"
#include "DBManager.h"
#include "InventoryView.h"
#include "AOMessageParsers.h"
#include "resource.h"
#include <map>
#include <set>
#include <shared/AODB.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <ShellAPI.h>

#define TIXML_USE_STL
#include <TinyXml/tinyxml.h>

using namespace WTL;


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

    m_toolbar.CheckButton(ID_INV_FIND_TOGGLE, TRUE);

    return 0;
}


LRESULT InventoryView::OnFindToggle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (!::IsWindowVisible(m_findview)) {
        m_findview.ShowWindow(SW_SHOW);
        m_findview.SetFocus();
        RECT rect;
        GetClientRect(&rect);
        CSize newsize(rect.right, rect.bottom);
        UpdateLayout(newsize);
    }
    else {
        HideFindWindow();
    } 

    m_toolbar.CheckButton(ID_INV_FIND_TOGGLE, m_findview.IsWindowVisible());

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

    m_toolbar.CheckButton(ID_INFO, m_infoview.IsWindowVisible());

    RECT rect;
    GetClientRect(&rect);
    CSize newsize(rect.right, rect.bottom);
    UpdateLayout(newsize);

    return 0;
}


LRESULT InventoryView::OnHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    SharedServices::ShowHelp(_T("inventory"));
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


LRESULT InventoryView::OnItemChanged(LPNMHDR lParam)
{
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam; 

    if (pnmv->uChanged & LVIF_STATE) {
        OnSelectionChanged();
    }

    return 0;
}


LRESULT InventoryView::OnItemContextMenu(LPNMHDR lParam)
{
    LPNMITEMACTIVATE param = (LPNMITEMACTIVATE)lParam;

    int sel = param->iItem;
    if (sel >= 0) {
        WTL::CPoint pos;
        GetCursorPos(&pos);

        WTL::CMenu menu;
        if (m_listview.GetSelectedCount() > 1) {
            menu.LoadMenu(IDR_ITEM_POPUP_MULTISEL);
        }
        else {
            menu.LoadMenu(IDR_ITEM_POPUP);
        }
        WTL::CMenuHandle popup = menu.GetSubMenu(0);
        int cmd = popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pos.x, pos.y, m_hWnd);
    }
    return 0;
}


LRESULT InventoryView::OnSellItemAoMarket(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled)
{
    if (m_listview.GetSelectedCount() != 1) {
        return 0;
    }
    int activeItemIdx = m_listview.GetSelectionMark();
    DWORD_PTR data = m_listview.GetItemData(activeItemIdx);

    std::tstring url = _T("http://www.aomarket.com/bots/additem?id=%lowid%&ql=%ql%");

    g_DBManager.Lock();
    OwnedItemInfoPtr pItemInfo = g_DBManager.GetOwnedItemInfo((int)data);
    g_DBManager.UnLock();

    boost::replace_all(url, _T("%lowid%"), pItemInfo->itemloid);
    boost::replace_all(url, _T("%ql%"), pItemInfo->itemql);

    ShellExecute(NULL, _T("open"), url.c_str(), NULL, NULL, SW_SHOWDEFAULT);

    return 0;
}


LRESULT InventoryView::OnCopyItemRef(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled)
{
    // Build the set of selected item indexes.
    std::set<int> selectedIndexes;
    if (m_listview.GetSelectedCount() < 1) {
        return 0;
    }
    else if (m_listview.GetSelectedCount() == 1) {
        selectedIndexes.insert(m_listview.GetNextItem(-1, LVNI_SELECTED));
    }
    else {
        int itemId = m_listview.GetNextItem(-1, LVNI_SELECTED);
        while (itemId >= 0) {
            selectedIndexes.insert(itemId);
            itemId = m_listview.GetNextItem(itemId, LVNI_SELECTED);
        }
    }

    ItemServer server;
    ExportFormat format;
    bool scriptOutput = false;

    switch (CommandId) {
        case ID_COPY_AUNO_ITEMREF:
            server = SERVER_AUNO;
            format = FORMAT_HTML;
            break;
        case ID_COPY_AUNO_ITEMREF_VBB:
            server = SERVER_AUNO;
            format = FORMAT_VBB;
            break;
        case ID_COPY_JAYDEE_ITEMREF:
            server = SERVER_JAYDEE;
            format = FORMAT_HTML;
            break;
        case ID_COPY_JAYDEE_ITEMREF_VBB:
            server = SERVER_JAYDEE;
            format = FORMAT_VBB;
            break;
        case ID_COPY_AO_ITEMREF:
            server = SERVER_AO;
            format = FORMAT_AO;
            scriptOutput = true;
            break;
        default:
            assert(false);  // Unknown command ID?
            break;
    }

    std::tstring urlTemplate = GetServerItemURLTemplate(server);
    std::tstring itemTemplate;
    std::tstring prefix;
    std::tstring postfix;
    std::tstring separator = _T("\r\n");

    switch (format) {
        case FORMAT_HTML:
            itemTemplate = _T("<a href=\"%url%\">%itemname%</a> [QL %ql%] [%itemlocation%]");
            separator = _T("<br />\r\n");
            break;
        case FORMAT_VBB:
            itemTemplate = _T("[url=%url%]%itemname%[/url] {QL %ql%} {%itemlocation%}");
            break;
        case FORMAT_AO:
            itemTemplate = _T("<a href=%url%>%itemname%</a> [QL %ql%] [%itemlocation%]");
            prefix = _T("<a href=\"text://");
            postfix = _T("<p><p><center>Generated by <a href='chatcmd:///start http://ia.frellu.net'>AOIA</a></center>\">Item List</a>");
            separator = _T("<br />");
            break;
        default:
            assert(false);  // Unknown type?
            break;
    }

    boost::replace_all(itemTemplate, _T("%url%"), urlTemplate);

    g_DBManager.Lock();
    std::tstring output = prefix;
    for (std::set<int>::iterator it = selectedIndexes.begin(); it != selectedIndexes.end(); ++it)
    {
        if (it != selectedIndexes.begin()) {
            output += separator;
        }

        DWORD_PTR data = m_listview.GetItemData(*it);
        OwnedItemInfoPtr pItemInfo = g_DBManager.GetOwnedItemInfo((unsigned int)data);

        std::tstring itemlocation = pItemInfo->ownername;
        itemlocation += _T(" -> ");
        itemlocation += pItemInfo->containername;

        std::tstring itemStr = itemTemplate;
        boost::replace_all(itemStr, _T("%lowid%"), pItemInfo->itemloid);
        boost::replace_all(itemStr, _T("%hiid%"), pItemInfo->itemhiid);
        boost::replace_all(itemStr, _T("%ql%"), pItemInfo->itemql);
        boost::replace_all(itemStr, _T("%itemname%"), pItemInfo->itemname);
        boost::replace_all(itemStr, _T("%itemlocation%"), itemlocation);

        output += itemStr;
    }
    output += postfix;
    g_DBManager.UnLock();

    if (!output.empty() && OpenClipboard() && EmptyClipboard()) {
        HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (output.length() * 2 + 2)); 
        if (hglbCopy == NULL) {
            CloseClipboard();
            return true;
        }

        // Lock the handle and copy the text to the buffer.
        TCHAR* lptstrCopy = (TCHAR*)GlobalLock(hglbCopy);
        ZeroMemory(lptstrCopy, output.length() * 2);
        memcpy(lptstrCopy, output.c_str(), output.length() * 2);
        GlobalUnlock(hglbCopy);

        // Place the handle on the clipboard.
#ifdef UNICODE
        SetClipboardData(CF_UNICODETEXT, hglbCopy);
#else
        SetClipboardData(CF_TEXT, hglbCopy);
#endif
        CloseClipboard();
    }

    // Output to the aoia script in the AO script folder.
    if (scriptOutput) {
        if (output.length() <= 4096) {
            std::tstring filename = STREAM2STR(g_DBManager.AOFolder() << _T("\\scripts\\aoia"));
#ifdef UNICODE
            std::wofstream ofs(filename.c_str());
#else
            std::ofstream ofs(filename.c_str());
#endif  // UNICODE
            ofs << output;

            static bool showMessage = true;

            if (showMessage)
            {
                MessageBox(
                    _T("All selected items were copied to the clipboard AND written to the AO script called 'aoia' \r\nYou can access it ingame by typing /aoia"), _T("INFORMATION"),
                    MB_OK | MB_ICONINFORMATION
                    );
                showMessage = false;
            }
        }
        else {
            MessageBox(
                _T("All selected items were copied to the clipboard, however Anarchy Online does not support scripts of more than 4096 bytes.\r\n\r\nTry selecting fewer items if you want the script file updated."), _T("ERROR"),
                MB_OK | MB_ICONERROR
                );
        }
    }

    return 0;
}


LRESULT InventoryView::OnShowItemRef(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled)
{
    if (m_listview.GetSelectedCount() != 1) {
        return 0;
    }
    int activeItemIdx = m_listview.GetSelectionMark();
    DWORD_PTR data = m_listview.GetItemData(activeItemIdx);

    g_DBManager.Lock();
    OwnedItemInfoPtr pItemInfo = g_DBManager.GetOwnedItemInfo((int)data);
    g_DBManager.UnLock();

    std::tstring itemlocation = pItemInfo->ownername;
    itemlocation += _T(" -> ");
    itemlocation += pItemInfo->containername;

    TCHAR buffer[1024];
    if (CommandId == ID_VIEW_ITEMSTATS_AUNO) {
        ATL::AtlLoadString(IDS_AUNO_ITEMREF_URL, buffer, 1024);
    }
    else if (CommandId == ID_VIEW_ITEMSTATS_JAYDEE) {
        ATL::AtlLoadString(IDS_JAYDEE_ITEMREF_URL, buffer, 1024);
    }
    else {
        assert(false); // Looks like you forgot to add a section to load the proper url for your command.
    }
    std::tstring url(buffer);

    if (!url.empty()) {
        boost::replace_all(url, _T("%lowid%"), pItemInfo->itemloid);
        boost::replace_all(url, _T("%hiid%"), pItemInfo->itemhiid);
        boost::replace_all(url, _T("%ql%"), pItemInfo->itemql);
    }

    ShellExecute(NULL, _T("open"), url.c_str(), NULL, NULL, SW_SHOWDEFAULT);

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

    m_toolbar.CheckButton(ID_INV_FIND_TOGGLE, FALSE);
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

    m_listview.Create(m_splitter.m_hWnd, rcDefault, NULL, style | LVS_REPORT /*| LVS_SINGLESEL*/ | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
    m_listview.SetDlgCtrlID(IDW_LISTVIEW);

    //m_treeview.SetUnicodeFormat();
    m_treeRoot.SetOwner(this);
    m_treeview.SetRootItem(&m_treeRoot);

    m_splitter.SetSplitterPanes(m_treeview, m_listview);
    m_splitter.SetActivePane(SPLIT_PANE_LEFT);

    m_accelerators.LoadAccelerators(IDR_INV_ACCEL);

    TBBUTTON buttons[5];
    buttons[0].iBitmap = 0;
    buttons[0].idCommand = ID_INV_FIND_TOGGLE;
    buttons[0].fsState = TBSTATE_ENABLED;
    buttons[0].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_CHECK | BTNS_AUTOSIZE;
    buttons[0].dwData = NULL;
    buttons[0].iString = (INT_PTR)_T("Find Item");
    buttons[1].iBitmap = 1;
    buttons[1].idCommand = ID_INFO;
    buttons[1].fsState = TBSTATE_ENABLED;
    buttons[1].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_CHECK | BTNS_AUTOSIZE;
    buttons[1].dwData = NULL;
    buttons[1].iString = (INT_PTR)_T("Item Info");
    buttons[2].iBitmap = 3;
    buttons[2].idCommand = 0;
    buttons[2].fsState = 0;
    buttons[2].fsStyle = BTNS_SEP;
    buttons[2].dwData = NULL;
    buttons[2].iString = NULL;
    buttons[3].iBitmap = 3;
    buttons[3].idCommand = ID_SELL_ITEM_AOMARKET;
    buttons[3].fsState = 0;
    buttons[3].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_AUTOSIZE;
    buttons[3].dwData = NULL;
    buttons[3].iString = (INT_PTR)_T("Sell Item");
    buttons[4].iBitmap = 2;
    buttons[4].idCommand = ID_HELP;
    buttons[4].fsState = TBSTATE_ENABLED;
    buttons[4].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_AUTOSIZE;
    buttons[4].dwData = NULL;
    buttons[4].iString = (INT_PTR)_T("Help");

    CImageList imageList;
    imageList.CreateFromImage(IDB_INVENTORY_VIEW, 16, 2, CLR_DEFAULT, IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

    m_toolbar.Create(GetTopLevelWindow(), NULL, _T("InventoryViewToolBar"), 
        ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 
        TBSTYLE_EX_MIXEDBUTTONS);
    m_toolbar.SetButtonStructSize();
    //m_toolbar.AddBitmap(2, IDB_INVENTORY_VIEW);
    m_toolbar.SetImageList(imageList);
    m_toolbar.AddButtons(ARRAYSIZE(buttons), buttons);
    m_toolbar.AutoSize();

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
                        unsigned int id = boost::lexical_cast<unsigned int>(pT->Data(row, col));
                        unsigned int charid = boost::lexical_cast<unsigned int>(pT->Data(row, 1));

                        data = ServicesSingleton::Instance()->GetContainerName(charid, id);
                    }
                    indx = m_listview.AddItem(indx, columnmap[column], data.c_str());
                }
                if (pT->Columns() > 0)
                {
                    int data = boost::lexical_cast<int>(pT->Data(row, 0));
                    m_listview.SetItemData(indx, data);
                }
            }
        }
    }

    m_listview.SortItemsEx(CompareStr, (LPARAM)this);

    OnSelectionChanged();
}


void InventoryView::OnSelectionChanged()
{
    int count = m_listview.GetSelectedCount();
    if (count == 1) {
        m_toolbar.EnableButton(ID_SELL_ITEM_AOMARKET, TRUE);
    }
    else {
        m_toolbar.EnableButton(ID_SELL_ITEM_AOMARKET, FALSE);
    }
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
            int a = boost::lexical_cast<int>(to_ascii_copy(name1));
            int b = boost::lexical_cast<int>(to_ascii_copy(name2));
            result = pThis->m_sortDesc ? b - a : a - b;
        }
        break;
    default:
        result = pThis->m_sortDesc ? StrCmpI(name2, name1) : StrCmpI(name1, name2);
    }

    return result;
}


std::tstring InventoryView::GetServerItemURLTemplate( ItemServer server )
{
    std::tstring retval;

    switch (server) {
        case SERVER_AUNO:
            {
                TCHAR buffer[1024];
                ATL::AtlLoadString(IDS_AUNO_ITEMREF_URL, buffer, 1024);
                retval = std::tstring(buffer);
            }
            break;
        case SERVER_JAYDEE:
            {
                TCHAR buffer[1024];
                ATL::AtlLoadString(IDS_JAYDEE_ITEMREF_URL, buffer, 1024);
                retval = std::tstring(buffer);
            }
            break;
        case SERVER_AO:
            {
                TCHAR buffer[1024];
                ATL::AtlLoadString(IDS_AO_ITEMREF_URL, buffer, 1024);
                retval = std::tstring(buffer);
            }
            break;
    }

    return retval;
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








FindView::FindView()
    : m_lastQueryChar(-1)
    , m_lastQueryQlMin(-1)
    , m_lastQueryQlMax(-1)
    , m_pParent(NULL)
{
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
            unsigned int id = boost::lexical_cast<unsigned int>(pT->Data(i,0));

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
    CEdit qlmin = GetDlgItem(IDC_QLMIN);
    CEdit qlmax = GetDlgItem(IDC_QLMAX);

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

    int minql = -1;
    ZeroMemory(buffer, MAX_PATH);
    qlmin.GetWindowText(buffer, MAX_PATH);
    std::tstring qlminText(buffer);
    try {
        minql = boost::lexical_cast<int>(qlminText);
    }
    catch(boost::bad_lexical_cast &/*e*/) {
        // Go with the default value
    }

    int maxql = -1;
    ZeroMemory(buffer, MAX_PATH);
    qlmax.GetWindowText(buffer, MAX_PATH);
    std::tstring qlmaxText(buffer);
    try {
        maxql = boost::lexical_cast<int>(qlmaxText);
    }
    catch(boost::bad_lexical_cast &/*e*/) {
        // Go with the default value
    }

    if ( text.size() > 2
        && ( m_lastQueryText != text || m_lastQueryChar != charid || m_lastQueryQlMin != minql || m_lastQueryQlMax != maxql ) )
    {
        m_lastQueryText = text;
        m_lastQueryChar = charid;
        m_lastQueryQlMin = minql;
        m_lastQueryQlMax = maxql;
        std::tstringstream sql;

        if (charid > 0) {
            sql << _T("owner = ") << charid << _T(" AND ");
        }
        if (minql > -1) {
            sql << _T("titems.ql >= ") << minql << _T(" AND ");
        }
        if (maxql > -1) {
            sql << _T("titems.ql <= ") << maxql << _T(" AND ");
        }

        sql << _T("keylow IN (SELECT aoid FROM aodb.tblAO WHERE name LIKE \"%") << text << _T("%\")");

        m_pParent->UpdateListView(sql.str());
    }
}
