#include "StdAfx.h"
#include "AOManager.h"
#include <boost/filesystem.hpp>
#include <Shared/UnicodeSupport.h>
#include <Shared/FileUtils.h>
#include <ItemAssistantCore/SettingsManager.h>


namespace bfs = boost::filesystem;
using namespace aoia;


SINGLETON_IMPL(AOManager);


AOManager::AOManager()
{
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



    //std::tstring path = getAOFolder();
    //path += _T("\\Prefs\\*");

    //WIN32_FIND_DATA findData;

    //HANDLE hFind = FindFirstFileEx(path.c_str(), FindExInfoStandard, &findData, FindExSearchLimitToDirectories, NULL, 0);

    //if (hFind != INVALID_HANDLE_VALUE)
    //{
    //    do
    //    {
    //        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
    //            && (findData.cFileName[0] != NULL) 
    //            && (findData.cFileName[0] != '.'))
    //        {
    //            result.push_back(std::tstring(findData.cFileName));
    //        }
    //    }
    //    while (FindNextFile(hFind, &findData));
    //    FindClose(hFind);
    //}

    return result;
}
