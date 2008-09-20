#pragma once

#include "MainFrm.h"

class Application
{
public:
    Application();
    ~Application();

    bool init(std::tstring const& cmdLine);
    void destroy();
    int run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT);

private:
    CMainFrame m_mainWindow;
};
