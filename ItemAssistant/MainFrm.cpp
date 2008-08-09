#include "stdafx.h"
#include "aboutdlg.h"
#include "InventoryView.h"
#include "MainFrm.h"
#include "shared/aopackets.h"
#include "InjectionSupport.h"
#include "ntray.h"

// Delay loaded function definition
typedef  BOOL (WINAPI *ChangeWindowMessageFilterFunc)(UINT message, DWORD dwFlag);
#define MSGFLT_ADD 1
#define MSGFLT_REMOVE 2


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    PluginViewInterface *plugin = m_tabbedChildWindow.GetActivePluginView();
    if (plugin != NULL)
    {
        if (plugin->PreTranslateMsg(pMsg))
        {
            return TRUE;
        }
    }

    if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
        return TRUE;

    //HWND hWnd = m_tabbedChildWindow.GetActiveView();
    //if (hWnd != NULL)
    //{
    //   return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg);
    //}

    return FALSE;
}


BOOL CMainFrame::OnIdle()
{
    UIUpdateToolBar();
    return FALSE;
}


LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    m_minimized = false;

    // create command bar window
    HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
    // attach menu
    m_CmdBar.AttachMenu(GetMenu());
    // load command bar images
    m_CmdBar.LoadImages(IDR_MAINFRAME);
    // remove old menu
    SetMenu(NULL);

    HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

    CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand(hWndCmdBar);
    AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

    CreateSimpleStatusBar();

    DWORD style = WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    m_tabbedChildWindow.SetTabStyles(CTCS_TOOLTIPS | CTCS_DRAGREARRANGE);
    m_hWndClient = m_tabbedChildWindow.Create(m_hWnd, rcDefault, 0, style | WS_VISIBLE);

    UIAddToolBar(hWndToolBar);

    SetToolbarVisibility(false);
    UISetCheck(ID_VIEW_STATUS_BAR, 1);

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    // Load a tray icon
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

    m_trayIcon = boost::shared_ptr<CTrayNotifyIcon>(new CTrayNotifyIcon());
    m_trayIcon->Create(this, IDR_TRAY_POPUP, _T("AO Item Assistant"), hIconSmall, WM_TRAYICON);

    // To allow incoming messages from lower level applications like the AO Client we need to add the
    // message we use to the exception list.
    // http://blogs.msdn.com/vishalsi/archive/2006/11/30/what-is-user-interface-privilege-isolation-uipi-on-vista.aspx
    // It is however not available in all versions of windows, so we need to check for it existance 
    // before we try to call it.
    {
        ChangeWindowMessageFilterFunc f = (ChangeWindowMessageFilterFunc)GetProcAddress(LoadLibrary(_T("user32.dll")), "ChangeWindowMessageFilter");
        if (f) {
            (f)(WM_COPYDATA, MSGFLT_ADD);
        }
    }

    Inject();
    SetTimer(1, 10000);

    return 0;
}


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (m_trayIcon) {
        m_trayIcon->RemoveIcon();
        m_trayIcon.reset();
    }
    PostMessage(WM_CLOSE);
    return 0;
}


LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: add code to initialize document

    return 0;
}


void CMainFrame::SetToolbarVisibility(bool visible)
{
    CReBarCtrl rebar = m_hWndToolBar;
    int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
    rebar.ShowBand(nBandIndex, visible);
    UISetCheck(ID_VIEW_TOOLBAR, visible);
    UpdateLayout();
}


LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    static BOOL bVisible = FALSE;	// initially not visible
    bVisible = !bVisible;
    SetToolbarVisibility(bVisible);
    return 0;
}


LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
    ::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
    UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
    UpdateLayout();
    return 0;
}


LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    static bool isInside = false;

    if (!isInside) {
        isInside = true;
        CAboutDlg dlg;
        dlg.DoModal();
        isInside = false;
    }

    return 0;
}


LRESULT CMainFrame::OnTrayShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (m_minimized) {
        SendMessage(WM_SYSCOMMAND, SC_RESTORE, NULL);
    }
    else {
        SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, NULL);
    }
    return 0;
}


LRESULT CMainFrame::OnAOMessage(HWND wnd, PCOPYDATASTRUCT pData)
{
    char* datablock = (char*)(pData->lpData);
    unsigned int datasize = pData->cbData;

    using namespace AO;

    Header* msg = (Header*)datablock;
    unsigned int msgId = _byteswap_ulong(msg->msgid);

    ATLASSERT(datasize == _byteswap_ushort(msg->msgsize));

    m_tabbedChildWindow.OnAOMessage(msg);

    return 0;
}


LRESULT CMainFrame::OnTimer(UINT wParam, TIMERPROC lParam)
{
    if (wParam == 1)
    {
        Inject();
        SetTimer(1, 10000);
    }
    return 0;
}


void CMainFrame::OnSysCommand(UINT wParam, CPoint mousePos)
{
    switch (wParam) {
    case SC_CLOSE:
    case SC_MINIMIZE:
    {
        DefWindowProc(WM_SYSCOMMAND, SC_MINIMIZE, NULL);
        ShowWindow(SW_HIDE); // Hides the task bar button.
        m_minimized = true;
    }
    break;
    case SC_RESTORE:
    {
        if (m_minimized) {
            ShowWindow(SW_SHOW);
            m_minimized = false;
        }
        DefWindowProc(WM_SYSCOMMAND, wParam, NULL);
    }
    break;
    default:
        SetMsgHandled(FALSE);
        break;
    }
}


void CMainFrame::Inject()
{
    HWND AOWnd;
    DWORD AOProcessId;
    HANDLE AOProcessHnd;

    if( AOWnd = FindWindow( _T("Anarchy client"), _T("Anarchy Online") ) )
    {
        // Get process id
        GetWindowThreadProcessId( AOWnd, &AOProcessId );

        TCHAR CurrDir[MAX_PATH];
        GetCurrentDirectory( MAX_PATH, CurrDir );

        // Inject the dll into client process
        std::tstringstream temp;
        temp << CurrDir << _T("\\ItemAssistantHook.dll");
        InjectDLL(AOProcessId, temp.str());
    }
}


LRESULT CMainFrame::OnTrayIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    if (m_trayIcon) {
        return m_trayIcon->OnTrayNotification(wParam, lParam);
    }
    return 0;
}
