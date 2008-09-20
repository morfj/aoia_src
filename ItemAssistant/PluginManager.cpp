#include "StdAfx.h"
#include "PluginManager.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace bfs = boost::filesystem;
namespace ba = boost::algorithm;


PluginManager::PluginManager()
{
}


PluginManager::~PluginManager()
{
}


void PluginManager::AddLibraries(std::tstring const& path)
{
    bfs::wpath dir(path);
    if (!bfs::is_directory(dir)) {
        return;
    }

    for (bfs::wdirectory_iterator it(dir); it != bfs::wdirectory_iterator(); ++it) {
        bfs::wpath dirEntry = *it;
        if (bfs::is_directory(dirEntry)) {
            continue;
        }
        if (ba::iends_with(dirEntry.leaf(), ".dll")) {
            // Try loading this DLL
            HMODULE hDll = ::LoadLibrary(dirEntry.string().c_str());
            if (!hDll) {
                continue;
            }

            // Look for exported plug-in factory function
            FARPROC fp = ::GetProcAddress(hDll, "AOIA_CreatePlugin");
            m_factories.push_back(fp);
        }
    }
}


std::vector<boost::shared_ptr<PluginViewInterface> > PluginManager::createPlugins()
{
    std::vector<boost::shared_ptr<PluginViewInterface> > retval;
    for (std::vector<FARPROC>::iterator it = m_factories.begin(); it != m_factories.end(); ++it) {
        AOIA_CreatePlugin f = (AOIA_CreatePlugin)*it;
        boost::shared_ptr<PluginViewInterface> plugin = f("");
        retval.push_back(plugin);
    }
    return retval;
}
