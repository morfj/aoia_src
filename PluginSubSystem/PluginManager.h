#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#ifndef PLUGINSUBSYSTEM_DLL
#pragma comment( lib, “PluginSubSystem.lib” )
#define PLUGINSUBSYSTEM_API __declspec(dllimport)
#else
#define PLUGINSUBSYSTEM_API __declspec(dllexport)
#endif

#include <vector>
#include <boost/smart_ptr.hpp>
#include <shared/UnicodeSupport.h>

class PLUGINSUBSYSTEM_API PluginManager
{
public:
    ~PluginManager();

    static boost::shared_ptr<PluginManager> Instance();
    static void DestroyInstance();

    void AddLibraries(std::tstring const& path);

    //std::vector<boost::shared_ptr<PluginViewInterface> > createPlugins();

private:
    PluginManager();                        // To prevent inheritance
    PluginManager(const PluginManager&);    // Hide copy constructor

    std::vector<FARPROC> m_factories;
    static boost::shared_ptr<PluginManager> ms_instance;
};

#endif // PLUGINMANAGER_H
