#include "StdAfx.h"
#include "AOManager.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <Shared/UnicodeSupport.h>
#include <Shared/FileUtils.h>
#include <ItemAssistantCore/SettingsManager.h>
#include "NetUtils.h"


namespace bfs = boost::filesystem;
using namespace aoia;


SINGLETON_IMPL(AOManager);


AOManager::AOManager()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}


AOManager::AOManager(const AOManager&)
{
}


AOManager::~AOManager()
{
}


std::tstring AOManager::getAOFolder() const
{
    if (m_aofolder.empty())
    {
        bfs::tpath AODir;
        bool requestFolder = true;

        // Get AO folder from settings
        std::tstring dir_setting = SettingsManager::instance().getValue(_T("AOPath"));
        if (!dir_setting.empty())
        {
            AODir = dir_setting;
            if (bfs::exists(AODir / _T("anarchy.exe")))
            {
                requestFolder = false;
            }
        }

        if (requestFolder)
        {
            AODir = BrowseForFolder(NULL, _T("Please locate the AO directory:"));
            if (AODir.empty()) {
                return _T("");
            }

            if (!bfs::exists(AODir / _T("anarchy.exe"))) {
                MessageBox( NULL, _T("This is not AO's directory."), _T("ERROR"), MB_OK | MB_ICONERROR);
                return _T("");
            }

            // Store the new AO directory in the settings
            SettingsManager::instance().setValue(_T("AOPath"), AODir.string());
        }

        m_aofolder = AODir.string();
    }

    return m_aofolder;
}


bool AOManager::createAOItemsDB(std::tstring const& localfile, bool showProgress)
{
    return false;
}


std::tstring AOManager::getCustomBackpackName(unsigned int charid, unsigned int containerid) const
{
    return _T("");
}


std::vector<std::tstring> AOManager::getAccountNames() const
{
    std::vector<std::tstring> result;

    bfs::path path(to_ascii_copy(getAOFolder()));
    path = path / "Prefs";

    if (bfs::exists(path)) {
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(path); itr != end_itr; ++itr ) {
            if (bfs::is_directory(itr->status())) {
                result.push_back(from_ascii_copy(itr->leaf()));
            }
        }
    }

    return result;
}


std::vector<AOManager::DimensionInfo> AOManager::getDimensions() const
{
    std::vector<AOManager::DimensionInfo> retval;

    bfs::path dimensionFile(to_ascii_copy(getAOFolder()));
    dimensionFile /= "cd_image/data/launcher/dimensions.txt";

    if (!bfs::exists(dimensionFile))
    {
        return retval;
    }

    std::ifstream ifs(dimensionFile.string().c_str());
    if (!ifs.is_open())
    {
        return retval;
    }

    DimensionInfo dimension;
    char buffer[1024];
    while (!ifs.eof())
    {
        ifs.getline(buffer, 1024, '\n');
        std::string line(buffer);
        if (line.length() < 3)
        {
            continue;
        }
        if (line[0] == '#')
        {
            continue;
        }
        if (line == "STARTINFO")
        {
            dimension.description = "";
            dimension.name = "";
            dimension.server_port = 0;
            dimension.server_ip.clear();
            continue;
        }
        if (line == "ENDINFO")
        {
            retval.push_back(dimension);
            continue;
        }
        size_t splitpos = line.find("=");
        if (splitpos == std::string::npos)
        {
            continue;
        }

        std::string left = line.substr(0, splitpos);
        std::string right = line.substr(splitpos + 1);

        boost::algorithm::trim(left);
        boost::algorithm::trim(right);

        if (left == "displayname")
        {
            dimension.description = right;
            continue;
        }
        if (left == "description")
        {
            dimension.name = right;
            continue;
        }
        if (left == "connect")
        {
            dimension.server_ip = NetUtils::LookupHost(right);
            continue;
        }
        if (left == "ports")
        {
            dimension.server_port = atoi(right.c_str());
            continue;
        }
    }

    return retval;
}
