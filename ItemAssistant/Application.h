#ifndef APPLICATION_H
#define APPLICATION_H

#include <settings/SettingsManager.h>

class CMainFrame;


class Application
{
public:
    Application();
    ~Application();

    bool init(std::tstring const& cmdLine);
    void destroy();
    int run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT);

private:
    boost::shared_ptr<CMainFrame> m_mainWindow;
    aoia::SettingsManager _settings;
};


#endif // APPLICATION_H
