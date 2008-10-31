#pragma once

#include "shared/aopackets.h"
#include <PluginSDK/ItemAssistView.h>
#include <atlsplit.h>
#include <vector>
#include "MFTreeView.h"
#include "InvTreeItems.h"


// Forward declarations
class InventoryView;


class InfoView
    : public CDialogImpl<InfoView>
    , public CDialogResize<InfoView>
{
public:
    enum { IDD = IDD_ITEM_INFO };

    InfoView();

    void SetParent(InventoryView* parent);
    BOOL PreTranslateMsg(MSG* pMsg);

    BEGIN_MSG_MAP(InfoView)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
        COMMAND_CODE_HANDLER(BN_CLICKED, OnButtonClicked)
        CHAIN_MSG_MAP(CDialogResize<InfoView>)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(InfoView)
        DLGRESIZE_CONTROL(IDC_LISTVIEW, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_BUTTON_LABEL, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_AUNO, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_JAYDEE, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    void SetCurrentItem(unsigned int item);

protected:
    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnButtonClicked(WORD commandID, WORD buttonID, HWND hButton, BOOL &bHandled);

private:
    InventoryView* m_pParent;
    unsigned int m_currentItem;
};


class FindView
    : public CDialogImpl<FindView>
    , public CDialogResize<FindView>
{
public:
    enum { IDD = IDD_INV_FIND };

    FindView();

    void SetParent(InventoryView* parent);
    BOOL PreTranslateMsg(MSG* pMsg);

    BEGIN_MSG_MAP(FindView)
        MSG_WM_TIMER(OnTimer)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
        COMMAND_HANDLER(IDC_ITEMTEXT, EN_CHANGE, OnEnChangeItemtext)
        COMMAND_HANDLER(IDC_QLMIN, EN_CHANGE, OnEnChangeItemtext)
        COMMAND_HANDLER(IDC_QLMAX, EN_CHANGE, OnEnChangeItemtext)
        COMMAND_HANDLER(IDC_CHARCOMBO, CBN_SELCHANGE, OnCbnSelChangeCharcombo)
        COMMAND_HANDLER(IDC_CHARCOMBO, CBN_SETFOCUS, OnCbnBuildCharcombo)
        CHAIN_MSG_MAP(CDialogResize<FindView>)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(FindView)
        DLGRESIZE_CONTROL(IDC_ITEMTEXT, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_STATIC_TOON, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(IDC_CHARCOMBO, DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCbnBuildCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnEnChangeItemtext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCbnSelChangeCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT wParam, TIMERPROC lParam);

protected:
    virtual void UpdateFindQuery();

private:
    InventoryView* m_pParent;
    std::tstring m_lastQueryText;
    int m_lastQueryChar;
    int m_lastQueryQlMin;
    int m_lastQueryQlMax;
};


class InventoryView
    : public ItemAssistView<InventoryView>
{
    typedef ItemAssistView<InventoryView> inherited;
public:
    DECLARE_WND_CLASS(NULL)

    InventoryView();
    virtual ~InventoryView();

    enum
    {
        WM_POSTCREATE = WM_APP + 1,
    };

    // Id numbers for child windows.
    enum {
        IDW_LISTVIEW = 1,
        IDW_TREEVIEW,
        IDW_FINDVIEW,
        IDW_INFOVIEW
    };

    BEGIN_MSG_MAP_EX(InventoryView)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        //MSG_WM_NOTIFY(OnNotify)
        MESSAGE_HANDLER(WM_POSTCREATE, OnPostCreate)
        COMMAND_ID_HANDLER(ID_INV_FIND_TOGGLE, OnFindToggle)
        COMMAND_ID_HANDLER(ID_INV_FIND, OnFind)
        COMMAND_ID_HANDLER(ID_INV_FIND_HIDE, OnFindHide)
        COMMAND_ID_HANDLER(ID_INFO, OnInfo)
        COMMAND_ID_HANDLER(ID_HELP, OnHelp)
        COMMAND_ID_HANDLER(ID_SELL_ITEM_AOMARKET, OnSellItemAoMarket)
        COMMAND_ID_HANDLER(ID_COPY_AUNO_ITEMREF, OnCopyItemRef)
        COMMAND_ID_HANDLER(ID_COPY_AUNO_ITEMREF_VBB, OnCopyItemRef)
        COMMAND_ID_HANDLER(ID_COPY_JAYDEE_ITEMREF, OnCopyItemRef)
        COMMAND_ID_HANDLER(ID_COPY_JAYDEE_ITEMREF_VBB, OnCopyItemRef)
        COMMAND_ID_HANDLER(ID_COPY_AO_ITEMREF, OnCopyItemRef)
        COMMAND_ID_HANDLER(ID_VIEW_ITEMSTATS_AUNO, OnShowItemRef)
        COMMAND_ID_HANDLER(ID_VIEW_ITEMSTATS_JAYDEE, OnShowItemRef)
        COMMAND_ID_HANDLER(ID_EXPORTTOCSV_AUNO, OnExportToCSV)
        COMMAND_ID_HANDLER(ID_EXPORTTOCSV_JAYDEE, OnExportToCSV)
        NOTIFY_CODE_HANDLER_EX(LVN_COLUMNCLICK, OnColumnClick)
        //NOTIFY_CODE_HANDLER_EX(LVN_ITEMACTIVATE, OnItemActivate)
        NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGED, OnItemChanged)
        NOTIFY_HANDLER_EX(IDW_LISTVIEW, NM_RCLICK, OnItemContextMenu)
        CHAIN_MSG_MAP(inherited)
        REFLECT_NOTIFICATIONS()
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    LRESULT OnCreate(LPCREATESTRUCT createStruct);
    LRESULT OnSize(UINT wParam, CSize newSize);
    LRESULT OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFindToggle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFindHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnColumnClick(LPNMHDR lParam);
    LRESULT OnItemChanged(LPNMHDR lParam);
    LRESULT OnItemContextMenu(LPNMHDR lParam);
    LRESULT OnSellItemAoMarket(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled);
    LRESULT OnCopyItemRef(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled);
    LRESULT OnShowItemRef(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled);
    LRESULT OnExportToCSV(WORD FromAccelerator, WORD CommandId, HWND hWndCtrl, BOOL& bHandled);

    virtual void OnAOServerMessage(AOMessageBase &msg);
    virtual bool PreTranslateMsg(MSG* pMsg);

    void HideFindWindow();
    void UpdateListView(std::tstring const& where);
    void OnSelectionChanged();

    enum ItemServer {
        SERVER_AUNO = 1,
        SERVER_JAYDEE,
        SERVER_AO,
    };

    static std::tstring GetServerItemURLTemplate( ItemServer server );

protected:
    enum ExportFormat {
        FORMAT_HTML = 1,
        FORMAT_VBB,
        FORMAT_AO,
        FORMAT_CSV,
    };

    //void AddItemToView(Native::DbKey const& key, Native::DbItem const& item);
    void AddToTreeView(unsigned int charId, unsigned int contId);
    void CleanupDB(unsigned int charid);
    void UpdateLayout(CSize newSize);
    std::vector<std::tstring> GetAccountNames();
    static int CALLBACK CompareStr(LPARAM param1, LPARAM param2, LPARAM sort);

private:
    CSplitterWindow   m_splitter;
    MFTreeView        m_treeview;
    WTL::CListViewCtrl     m_listview;
    FindView          m_findview;
    InfoView          m_infoview;

    CTreeItem   m_inventory;
    CTreeItem   m_bank;
    CTreeItem   m_unknown;

    CAccelerator m_accelerators;

    //std::vector<std::string> m_accounts;

    InventoryTreeRoot m_treeRoot;

    bool  m_sortDesc;
    int   m_sortColumn;
};
