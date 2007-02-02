// ItemAssistant.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ItemAssistant.h"
#include "DBManager.h"
#include <shared/localdb.h>
#include <shlobj.h>
#include "MainFrm.h"


#define MAX_LOADSTRING 100


// Global Variables:
CAppModule _Module;
DBManager g_DBManager;


int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainFrame wndMain;

    if(wndMain.CreateEx() == NULL) {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    ServicesSingleton::Instance()->SetTrayIcon(wndMain.GetTrayIcon());

    wndMain.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();
    return nRet;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, hInstance, &LIBID_ATLLib);
    ATLASSERT(SUCCEEDED(hRes));

    AtlAxWinInit();

    std::tstring args( lpCmdLine );
    std::tstring::size_type argPos = args.find(_T("-db"));

    std::tstring dbfile;
    if (argPos != std::tstring::npos)
    {
        dbfile = args.substr(argPos+4, args.find_first_of(_T(" "), argPos+4)-argPos-4);
    }

    int nRet = -1;
    if (g_DBManager.Init(dbfile))
    {
        nRet = Run(lpCmdLine, nCmdShow);

        g_DBManager.Lock();
        g_DBManager.Term();
        g_DBManager.UnLock();

        _Module.Term();
        ::CoUninitialize();
    }

    return nRet;
}
