#include "StdAfx.h"
#include "Application.h"


Application::Application()
{
}


Application::~Application()
{
}


bool Application::init(std::tstring const& cmdLine)
{
    // Check to see if logging should be enabled
    if (cmdLine.find(_T("-log")) != std::tstring::npos) {
        Logger::instance()->init(_T("ItemAssistant.log"));
    }
    else {
        Logger::instance()->init();
    }

    std::tstring dbfile;
    std::tstring::size_type argPos = cmdLine.find(_T("-db"));
    if (argPos != std::tstring::npos) {
        dbfile = cmdLine.substr(argPos+4, cmdLine.find_first_of(_T(" "), argPos+4)-argPos-4);
    }

    if (!g_DBManager.Init(dbfile)) {
        return false;
    }

    return true;
}


void Application::destroy()
{
    g_DBManager.Lock();
    g_DBManager.Term();
    g_DBManager.UnLock();

    Logger::instance()->destroy();
}


int Application::run(LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    if(m_mainWindow.CreateEx() == NULL) {
        assert(false);  // Could not create main window for some reason.
        return 0;
    }

    ServicesSingleton::Instance()->SetTrayIcon(m_mainWindow.GetTrayIcon());

    m_mainWindow.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();
    return nRet;
}
