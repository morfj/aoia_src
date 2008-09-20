#pragma once

#include <boost/smart_ptr.hpp>
#include <PluginSDK/PluginViewInterface.h>


class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    void AddLibraries(std::tstring const& path);

    std::vector<boost::shared_ptr<PluginViewInterface> > createPlugins();

private:
    std::vector<FARPROC> m_factories;
};
