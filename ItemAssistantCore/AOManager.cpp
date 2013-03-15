#include "StdAfx.h"
#include "AOManager.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <Shared/UnicodeSupport.h>
#include <Shared/FileUtils.h>
#include <Shlobj.h>
#include <iomanip>
#include <shared/AOPathHasher.h>


namespace bfs = boost::filesystem;
namespace ba = boost::algorithm;
using namespace aoia;

namespace {

    bfs::tpath getAOLocalAppDataFolder()
    {
        TCHAR localAppData[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, localAppData);
        bfs::tpath path(localAppData);
        path /= _T("Funcom");
        path /= _T("Anarchy Online");
        return path;
    }

    bfs::tpath getPreferenceLocationFromInstallPath( std::tstring const& aoInstallationFolder ) 
    {
        bfs::tpath ao_path(aoInstallationFolder);

        std::tostringstream hash;
        hash << std::hex << std::setw(8) << std::setfill(_T('0')) << shared::HashParentPath(ao_path);

        return getAOLocalAppDataFolder() / hash.str() / ao_path.filename() / _T("Prefs");
    }
}


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
    assert(m_settings);

    if (m_aofolder.empty())
    {
        bfs::tpath AODir;
        bool requestFolder = true;

        // Get AO folder from settings
        std::tstring dir_setting = m_settings->getValue(_T("AOPath"));
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
                MessageBox( NULL, 
                    _T("The specified directory doesn't contain a valid Anarchy Online installation."),
                    _T("Error - AO Item Assistant"), 
                    MB_OK | MB_ICONERROR);
                return _T("");
            }

            // Store the new AO directory in the settings
            m_settings->setValue(_T("AOPath"), AODir.native());
        }

        m_aofolder = AODir.native();
    }

    return m_aofolder;
}


std::tstring AOManager::getAOPrefsFolder() const
{
    assert(m_settings);

    if (m_prefsfolder.empty()) {
        bool requestFolder = true;
        bfs::tpath prefsDir;

        // Get prefs folder override from settings
        std::tstring dir_setting = m_settings->getValue(_T("AOPrefsPath"));
        if (!dir_setting.empty()) {
            prefsDir = dir_setting;
            if (bfs::exists(prefsDir / _T("Prefs.xml"))) {
                requestFolder = false;
            }
        }

        if (requestFolder) {
            // We intentionally do NOT store this in settings. Settings should be for an override only.
            prefsDir = getPreferenceLocationFromInstallPath(getAOFolder());
        }

        m_prefsfolder = prefsDir.native();
    }

    return m_prefsfolder;
}


std::vector<std::tstring> AOManager::getAccountNames() const
{
    std::vector<std::tstring> result;

    bfs::path path(to_ascii_copy(getAOPrefsFolder()));

    if (bfs::exists(path)) {
        bfs::directory_iterator end_itr; // default construction yields past-the-end
        for (bfs::directory_iterator itr(path); itr != end_itr; ++itr ) {
            if (bfs::is_directory(itr->status())) {
                if (itr->path().leaf() != "Browser") {
                    result.push_back(itr->path().leaf().c_str());
                }
            }
        }
    }

    return result;
}


void AOManager::SettingsManager( aoia::ISettingsPtr settings )
{
    m_settings = settings;
}
