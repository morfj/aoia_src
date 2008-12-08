#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#ifndef PLUGINSUBSYSTEM_DLL
#pragma comment( lib, "PluginSubSystem.lib" )
#define PLUGINSUBSYSTEM_API __declspec(dllimport)
#else
#define PLUGINSUBSYSTEM_API __declspec(dllexport)
#endif

#include <vector>
#include <boost/smart_ptr.hpp>
#include <shared/UnicodeSupport.h>
#include <Shared/Singleton.h>

class PLUGINSUBSYSTEM_API PluginManager
{
    SINGLETON(PluginManager);
public:
    ~PluginManager();

    void AddLibraries(std::tstring const& path);
    //std::vector<boost::shared_ptr<PluginViewInterface> > createPlugins();

private:
    std::vector<FARPROC> m_factories;
};

#endif // PLUGINMANAGER_H
