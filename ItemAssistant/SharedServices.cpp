#include "StdAfx.h"
#include "SharedServices.h"

#define TIXML_USE_STL
#include <TinyXml/tinyxml.h>

#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include "DBManager.h"
#include "MainFrm.h"
#include "ntray.h"


SharedServices::SharedServices()
{
    m_accounts = GetAccountNames();
}


SharedServices::~SharedServices()
{
}


std::tstring SharedServices::GetContainerName(unsigned int charid, unsigned int containerid) const
{
    if (containerid == 1)
    {
        return _T("Bank");
    }
    else if (containerid == 2)
    {
        return _T("Inventory/Equip");
    }

    std::tstring result;

    __int64 key = ((__int64)charid) << 32;
    key += containerid;


    FILETIME lastWrite;
    lastWrite.dwHighDateTime = lastWrite.dwLowDateTime = 0;

    std::tstring filename;
    for (unsigned int i = 0; i < m_accounts.size(); i++)
    {
        filename = STREAM2STR( g_DBManager.AOFolder() << _T("\\Prefs\\") << m_accounts[i] << _T("\\Char") << charid << _T("\\Containers\\Container_51017x") << containerid << _T(".xml") );
        if (PathFileExists(filename.c_str()))
        {
            WIN32_FILE_ATTRIBUTE_DATA atribs; 
            if (GetFileAttributesEx(filename.c_str(), GetFileExInfoStandard, &atribs))
            {
                lastWrite = atribs.ftLastWriteTime;
                break;
            }
        }
    }

    bool wasInCache = m_containerFileCache.find(key) != m_containerFileCache.end();

    // Clear invalid cache
    if ((filename.empty() || (lastWrite.dwHighDateTime == 0 && lastWrite.dwHighDateTime == 0)) && wasInCache)
    {
        m_containerFileCache.erase(m_containerFileCache.find(key));
    }

    // Create cache from file
    if (!filename.empty())
    {
        bool update = true;

        // If already in cache, check timestamps
        if (m_containerFileCache.find(key) != m_containerFileCache.end())
        {
            FILETIME stamp = m_containerFileCache[key].second;
            if (stamp.dwHighDateTime == lastWrite.dwHighDateTime &&
                stamp.dwLowDateTime == lastWrite.dwLowDateTime)
            {
                update = false;
                result = m_containerFileCache[key].first;
            }
        }

        if (update)
        {
            TiXmlDocument document;
            if (document.LoadFile(to_ascii_copy(filename), TIXML_ENCODING_UTF8))
            {
                TiXmlHandle docHandle( &document );
                TiXmlElement* element = docHandle.FirstChild( "Archive" ).FirstChild( "String" ).Element();

                while (element)
                {
                    if (StrCmpA(element->Attribute("name"), "container_name") == 0)
                    {
                        result = from_utf8_copy(element->Attribute("value"));
                        m_containerFileCache[key] = std::pair<std::tstring, FILETIME>(result, lastWrite);
                        break;
                    }
                    element = element->NextSiblingElement();
                }
            }
        }
    }


    if (result.empty())
    {
        result = MakeContainerName(charid, containerid);
    }

    return result;
}


std::tstring SharedServices::MakeContainerName(unsigned int charid, unsigned int containerid) const
{
    __int64 key = ((__int64)charid) << 32;
    key += containerid;

    // Return cached value if exist
    if (m_containerDBCache.find(key) != m_containerDBCache.end())
    {
        return m_containerDBCache[key];
    }

    std::tstring result;

    // Lets find the item description for the specified container.
    g_DBManager.Lock();
    SQLite::TablePtr pT2 = g_DBManager.ExecTable( STREAM2STR( "SELECT keylow from tItems WHERE children = " << containerid ));
    g_DBManager.UnLock();

    if (pT2 != NULL)
    {
        int keylow = boost::lexical_cast<int>(pT2->Data(0,0));

        std::map<std::tstring, std::tstring> item = GetAOItemInfo(keylow);
        result = item[_T("name")];
    }
    else
    {
        result = STREAM2STR( "Backpack: " << containerid );
    }

    m_containerDBCache[key] = result;

    return result;
}


/**
 * Returns all columns from the AO Item database for the specified key value. 
 * The items are returned in a map where the key is the name of the property,
 * and the value is the string representation of the property value.
 * The following properties are currently available:
 *    aoid, ql, type, fromaoid, name
 */
std::map<std::tstring, std::tstring> SharedServices::GetAOItemInfo(unsigned int lowkey) const
{
    std::map<std::tstring, std::tstring> result;

    std::tstringstream sql;
    sql << _T("SELECT aoid, ql, type, fromaoid, name FROM tblAO WHERE aoid = ") << lowkey;

    g_DBManager.Lock();
    SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
    g_DBManager.UnLock();

    if (pT != NULL)
    {
        if (pT->Rows() > 0)
        {
            for (unsigned int col = 0; col < pT->Columns(); col++)
            {
                std::tstring column = from_ascii_copy(pT->Headers()[col]);
                std::tstring data = boost::algorithm::trim_copy(from_ascii_copy(pT->Data()[col]));

                result[column] = data;
            }
        }
    }

    return result;
}


std::vector<std::tstring> SharedServices::GetAccountNames() const
{
    std::vector<std::tstring> result;

    std::tstring path = g_DBManager.AOFolder();
    path += _T("\\Prefs\\*");

    WIN32_FIND_DATA findData;

    HANDLE hFind = FindFirstFileEx(path.c_str(), FindExInfoStandard, &findData, FindExSearchLimitToDirectories, NULL, 0);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                && (findData.cFileName[0] != NULL) 
                && (findData.cFileName[0] != '.'))
            {
                result.push_back(std::tstring(findData.cFileName));
            }
        }
        while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }

    return result;
}


void SharedServices::ShowTrayIconBalloon(std::tstring const& message) const
{
    m_trayIcon->SetBalloonDetails(message.c_str(), _T("AO Item Assistant"), CTrayNotifyIcon::Info, 5000);
}


void SharedServices::SetTrayIcon(boost::shared_ptr<CTrayNotifyIcon> trayIcon)
{
    m_trayIcon = trayIcon;
}
