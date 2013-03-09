#include "StdAfx.h"
#include "AOManager.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <Shared/UnicodeSupport.h>
#include <Shared/FileUtils.h>
#include <Shlobj.h>

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

    std::vector<bfs::tpath> findAllMatchingFilesInFolder(bfs::tpath rootFolder, std::tstring fileName)
    {
        std::vector<bfs::tpath> matchingPaths;

        bfs::directory_iterator end_itr; // Default ctor yields past-the-end
        for (bfs::directory_iterator it(rootFolder); it != end_itr; ++it)
        {
            // Skip if not a file
            if (bfs::is_directory(it->status())) {
                std::vector<bfs::tpath> paths = findAllMatchingFilesInFolder(*it, fileName);
                matchingPaths.insert(matchingPaths.end(), paths.begin(), paths.end());
                continue;
            }

            if (bfs::is_regular_file(it->status()) && it->path().filename().native() == fileName) {
                matchingPaths.push_back(*it);
            }
        }

        return matchingPaths;
    }

    std::vector<bfs::tpath> findAllPrefsFolders(std::tstring const& aoInstallationFolder)
    {
        std::vector<bfs::tpath> allPrefsFiles = findAllMatchingFilesInFolder(getAOLocalAppDataFolder(), _T("Prefs.xml"));

        bfs::tpath installPath(aoInstallationFolder);
        std::tstring folder = installPath.filename().native();
        std::tstring pattern = (getAOLocalAppDataFolder() / _T("[a-zA-Z0-9]{8}") / folder / _T("Prefs") / _T("Prefs.xml")).native();
        ba::replace_all(pattern, _T("\\"), _T("\\\\"));
        ba::to_upper(pattern);
        const boost::basic_regex<TCHAR, boost::regex_traits<TCHAR>> filter(pattern);

        boost::wsmatch what;
        std::vector<bfs::tpath> matchingPaths;
        for (std::vector<bfs::tpath>::const_iterator it = allPrefsFiles.begin(); it != allPrefsFiles.end(); ++it) {
            if (boost::regex_match(ba::to_upper_copy((*it).native()), what, filter)) {
                matchingPaths.push_back(it->parent_path());
            }
        }

        return matchingPaths;
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

        // Get prefs folder from settings
        std::tstring dir_setting = m_settings->getValue(_T("PrefsPath"));
        if (!dir_setting.empty()) {
            prefsDir = dir_setting;
            if (bfs::exists(prefsDir / _T("Prefs.xml"))) {
                requestFolder = false;
            }
        }

        if (requestFolder) {
            // Check to see if we can be a smart-ass and find a single prefs folder.
            std::vector<bfs::tpath> prefsLocations = findAllPrefsFolders(getAOFolder());
            if (prefsLocations.size() == 1) {
                prefsDir = prefsLocations.front();
                m_settings->setValue(_T("PrefsPath"), prefsDir.native());
            }
        }

        //give up and prompt
        if (prefsDir.empty()) {
            prefsDir = BrowseForFolder(NULL, _T("Please locate the AO preference folder:"));
            if (prefsDir.empty()) {
                return _T("");
            }

            if (!bfs::exists(prefsDir / _T("Prefs.xml"))) {
                MessageBox( NULL, 
                    _T("The specified directory doesn't seem to contain a valid Anarchy Online preference folder structure."), 
                    _T("Error - AO Item Assistant"), 
                    MB_OK | MB_ICONERROR);
                return _T("");
            }
            // Store the new AO directory in the settings
            m_settings->setValue(_T("PrefsPath"), prefsDir.native());
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
