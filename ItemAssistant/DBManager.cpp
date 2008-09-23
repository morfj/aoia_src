#include "StdAfx.h"
#include "dbmanager.h"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <shared/AODatabaseParser.h>
#include <shared/AODatabaseWriter.h>
#include <sstream>
#include "ProgressDialog.h"


namespace bfs = boost::filesystem;


/*********************************************************/
/* DB Manager Implementation                             */
/*********************************************************/


DBManager::DBManager(void)
{
}


DBManager::~DBManager(void)
{
}


bool DBManager::Init(std::tstring dbfile)
{
    // Init the Copy of AO DB

    std::tstring AODir;
    bool requestFolder = true;

    ATL::CRegKey regKey;
    if (regKey.Open(HKEY_CURRENT_USER, _T("Software\\AOItemAssistant"), KEY_ALL_ACCESS) == ERROR_SUCCESS)
    {
        TCHAR ao_dir[MAX_PATH];
        ULONG ao_dir_size = MAX_PATH;
        ZeroMemory(ao_dir, MAX_PATH * sizeof(TCHAR));

        if (regKey.QueryStringValue(_T("AOPath"), ao_dir, &ao_dir_size) == ERROR_SUCCESS)
        {
            AODir = std::tstring(ao_dir);
            std::tstringstream AOExePath;
            AOExePath << AODir << _T("\\anarchy.exe");

            FILE* fp;
            //char AOExePath[300];
            //sprintf( AOExePath, "%s\\anarchy.exe", AODir.c_str() );
            if( fopen_s( &fp, to_ascii_copy(AOExePath.str()).c_str(), "r" ) == S_OK )
            {
                requestFolder = false;
                fclose( fp );
            }
        }
        regKey.Close();
    }
    else
    {
        if (regKey.Create(HKEY_CURRENT_USER, _T("Software\\AOItemAssistant")) == ERROR_SUCCESS)
        {
            regKey.Close();
        }
    }

    if (requestFolder)
    {
        AODir = GetFolder(NULL, _T("Please locate the AO directory:"));
        if (AODir.empty()) {
            return false;
        }

        bfs::path pathOfExe(to_ascii_copy(AODir));
        pathOfExe = pathOfExe / "anarchy.exe";
        if (!bfs::exists(pathOfExe)) {
            MessageBox( NULL, _T("This is not AO's directory."), _T("ERROR"), MB_OK | MB_ICONERROR);
            return false;
        }

        if (regKey.Open(HKEY_CURRENT_USER, _T("Software\\AOItemAssistant"), KEY_ALL_ACCESS) == ERROR_SUCCESS)
        {
            regKey.SetStringValue(_T("AOPath"), AODir.c_str());
            regKey.Close();
        }
    }

    m_aofolder = AODir;

    if (dbfile.empty()) {
        dbfile = _T("ItemAssistant.db");
    }

    bool dbfileExists = bfs::exists(to_ascii_copy(dbfile));

    if (!SQLite::Db::Init(dbfile))
    {
        return false;
    }

    if (!dbfileExists) {
        CreateDBScheme();
    }

    unsigned int dbVersion = GetDBVersion();
    if (dbVersion < 1) {
        if (IDOK != MessageBox( NULL, _T("AO Item Assistant needs to update its database file to a newer version."), 
            _T("Question - AO Item Assistant"), MB_OKCANCEL | MB_ICONQUESTION))
        {
            return false;
        }
        UpdateDBVersion(dbVersion);
    }
    else if (dbVersion > 1)
    {
        MessageBox( NULL, _T("AO Item Assistant has detected a too new version of its database file. You should upgrade the software to continue."),
            _T("Error - AO Item Assistant"), MB_OK | MB_ICONERROR);
        return false;
    }

    if (!SyncLocalItemsDB(_T("aoitems.db"), m_aofolder)) {
        MessageBox( NULL, _T("AO Item Assistant can not start without a valid item database."),
            _T("Error - AO Item Assistant"), MB_OK | MB_ICONERROR);
        return false;
    }

    Exec(_T("ATTACH DATABASE \"aoitems.db\" AS aodb"));

    return true;
}


void DBManager::Term()
{
    SQLite::Db::Term();
}


std::tstring DBManager::GetFolder(HWND hWndOwner, std::tstring const& title)
{
    BROWSEINFO udtBI;
    ITEMIDLIST *udtIDList;

    /* Initialise */
    udtBI.hwndOwner = hWndOwner;
    udtBI.pidlRoot = NULL;
    udtBI.pszDisplayName = NULL;
    udtBI.lpszTitle = title.c_str();
    udtBI.ulFlags = BIF_RETURNONLYFSDIRS;
    udtBI.lpfn = NULL;
    udtBI.lParam = NULL;
    udtBI.iImage = 0;

    /* Prompt user for folder */
    udtIDList = SHBrowseForFolder(&udtBI);

    /* Extract pathname */
    TCHAR strPath[MAX_PATH];
    if (!SHGetPathFromIDList(udtIDList, (TCHAR*)&strPath)) {
        strPath[0] = 0;	// Zero-length if failure
    }

    return std::tstring(strPath);
}


/** 
 * Check to see if we have a local database already.
 * If it is, then check if it is up to date.
 * If local database is missing or obsolete then recreate it.
 * Return true if application has a local items database to run with, false otherwise.
*/
bool DBManager::SyncLocalItemsDB(std::tstring const& localfile, std::tstring const& aofolder)
{
    bool hasLocalDB = false;
    std::time_t lastUpdateTime;

    bfs::path local(to_ascii_copy(localfile));
    bfs::path original(to_ascii_copy(aofolder));
    original = original / "cd_image/data/db/ResourceDatabase.dat";
    
    if (bfs::exists(local) && bfs::is_regular(local)) {
        hasLocalDB = true;
        lastUpdateTime = bfs::last_write_time(local);
    }

    if (!exists(original)) {
        Logger::instance()->log(_T("Could not locate the original AO database."));
        return hasLocalDB;
    }

    if (hasLocalDB && GetAODBSchemeVersion(localfile) == 1) {
        std::time_t lastOriginalUpdateTime = bfs::last_write_time(original);
        if (lastOriginalUpdateTime <= lastUpdateTime) {
                return true;
        }

        // Ask user if he wants to continue using the old (but compatible) DB or update it now.
        int answer = ::MessageBox(NULL, 
            _T("You items database is out of date. Do you wish to update it now?\r\nAnswering 'NO' will continue using the old one."),
            _T("Question - AO Item Assistant"), MB_ICONQUESTION | MB_YESNOCANCEL);
        if (answer == IDCANCEL) {
            exit(0);
        }
        else if (answer == IDNO) {
            return true;
        }
    }

    // If we come this far we need to update the DB.

    bfs::path tmpfile("tmp_" + local.string());
    bfs::remove(tmpfile);

    try {
        AODatabaseParser aodb(original.string());
        AODatabaseWriter writer(tmpfile.string());

        unsigned int itemCount = aodb.GetItemCount(AODB_TYP_ITEM);
        unsigned int nanoCount = aodb.GetItemCount(AODB_TYP_NANO);

        CProgressDialog dlg(itemCount + nanoCount, itemCount);
        dlg.SetWindowText(_T("Progress Dialog - Item Assistant"));
        dlg.setText(0, _T("Extracting data from the AO DB..."));
        dlg.setText(1, STREAM2STR("Finished " << 0 << " out of " << itemCount << " items."));
        dlg.setText(2, _T("Overall progress: 0%"));

        // Extract items
        boost::shared_ptr<ao_item> item = aodb.GetFirstItem(AODB_TYP_ITEM);
        if (item) {
            unsigned int count = 1;
            writer.BeginWrite();
            writer.WriteItem(item);
            while (item = aodb.GetNextItem()) { 
                ++count; 
                writer.WriteItem(item);
                if (count % 1000 == 0) {
                    if (dlg.userCanceled()) {
                        return false;
                    }
                    dlg.setTaskProgress(count, itemCount);
                    dlg.setText(1, STREAM2STR("Finished " << count << " out of " << itemCount << " items."));
                    dlg.setOverallProgress(count, itemCount + nanoCount);
                    dlg.setText(2, STREAM2STR("Overall progress: " << (count * 100) / (itemCount + nanoCount) << "%"));
                }
                if (count % 10000 == 0) {
                    writer.CommitItems();
                    writer.BeginWrite();
                }
            }
            dlg.setTaskProgress(count, itemCount);
            dlg.setText(1, STREAM2STR("Finished " << count << " out of " << itemCount << " items."));
            dlg.setOverallProgress(count, itemCount + nanoCount);
            dlg.setText(2, STREAM2STR("Overall progress: " << (count * 100) / (itemCount + nanoCount) << "%"));
            writer.CommitItems();
        }

        if (dlg.userCanceled()) {
            return false;
        }
        //dlg.setTaskProgress(0, nanoCount);
        //dlg.setText(1, STREAM2STR("Finished " << 0 << " out of " << nanoCount << " nanos."));
        //dlg.setText(2, STREAM2STR("Overall progress: " << (itemCount * 100) / (itemCount + nanoCount) << "%"));
        //dlg.setOverallProgress(itemCount, itemCount + nanoCount);

        // Extract nano programs
        boost::shared_ptr<ao_item> nano = aodb.GetFirstItem(AODB_TYP_NANO);
        if (nano) {
            unsigned int count = 1;
            writer.BeginWrite();
            writer.WriteItem(nano);
            while (nano = aodb.GetNextItem()) { 
                ++count; 
                writer.WriteItem(nano);
                if (count % 1000 == 0) {
                    if (dlg.userCanceled()) {
                        return false;
                    }
                    dlg.setTaskProgress(count, nanoCount);
                    dlg.setText(1, STREAM2STR("Finished " << count << " out of " << nanoCount << " nanos."));
                    dlg.setOverallProgress(itemCount + count, itemCount + nanoCount);
                    dlg.setText(2, STREAM2STR("Overall progress: " << ((itemCount + count) * 100) / (itemCount + nanoCount) << "%"));
                }
                if (count % 10000 == 0) {
                    writer.CommitItems();
                    writer.BeginWrite();
                }
            }
            dlg.setTaskProgress(count, nanoCount);
            dlg.setText(1, STREAM2STR("Finished " << count << " out of " << nanoCount << " nanos."));
            dlg.setOverallProgress(itemCount + count, itemCount + nanoCount);
            dlg.setText(2, STREAM2STR("Overall progress: " << ((itemCount + count) * 100) / (itemCount + nanoCount) << "%"));
            writer.CommitItems();
        }

        if (dlg.userCanceled()) {
            return false;
        }
        //dlg.setTaskProgress(nanoCount, nanoCount);
        //dlg.setOverallProgress(itemCount + nanoCount, itemCount + nanoCount);
        //dlg.setText(1, STREAM2STR("Finished " << nanoCount << " out of " << nanoCount << " nanos."));
        //dlg.setText(2, _T("Overall progress: 100%"));

        writer.PostProcessData();
    }
    catch (std::bad_alloc &e) {
        assert(false);
        Logger::instance()->log(STREAM2STR(_T("Error creating item database. ") << e.what()));
        return false;
    }
    catch (AODatabaseParser::CTreeDbException &e) {
        assert(false);
        Logger::instance()->log(STREAM2STR(_T("Error parsing the AO Database. ") << e.what()));
        MessageBox( NULL, _T("Unable to parse the AO database.\n\rPlease make sure Anarchy Online is not running and try again."),
            _T("Error - AO Item Assistant"), MB_OK | MB_ICONERROR);
        return false;
    }
    catch (std::exception &e) {
        assert(false);
        Logger::instance()->log(STREAM2STR(_T("Error creating item database. ") << e.what()));
        return false;
    }

    remove(local);
    rename(tmpfile, local);

    return true;
}


void DBManager::InsertItem(unsigned int keylow,
                           unsigned int keyhigh,
                           unsigned short ql,
                           unsigned short stack,
                           unsigned int parent,
                           unsigned short slot,
                           unsigned int children,
                           unsigned int owner)
{
    std::tstringstream sql;
    sql << _T("INSERT INTO tItems (keylow, keyhigh, ql, stack, parent, slot, children, owner) VALUES (")
        << (unsigned int) keylow      << _T(", ")
        << (unsigned int) keyhigh     << _T(", ")
        << (unsigned int) ql          << _T(", ")
        << (unsigned int) stack       << _T(", ")
        << (unsigned int) parent      << _T(", ")
        << (unsigned int) slot        << _T(", ")
        << (unsigned int) children    << _T(", ")
        << (unsigned int) owner       << _T(")");
    Exec(sql.str());
}


std::tstring DBManager::GetToonName(unsigned int charid) const
{
    std::tstring result;

    SQLite::TablePtr pT = g_DBManager.ExecTable(STREAM2STR("SELECT charid, charname FROM tToons WHERE charid = " << charid));

    if (pT != NULL && pT->Rows())
    {
        if (!pT->Data()[1].empty())
        {
            result = from_ascii_copy(pT->Data()[1]);
        }
    }

    return result;
}


void DBManager::SetToonName(unsigned int charid, std::tstring const& newName)
{
    g_DBManager.Begin();

    {
        g_DBManager.Exec(STREAM2STR("DELETE FROM tToons WHERE charid = " << charid));
    }
    {
        g_DBManager.Exec(STREAM2STR("INSERT INTO tToons (charid, charname) VALUES (" << charid << ", '" << newName.c_str() << "')"));
    }

    g_DBManager.Commit();
}


OwnedItemInfoPtr DBManager::GetOwnedItemInfo(unsigned int itemID)
{
    OwnedItemInfoPtr pRetVal(new OwnedItemInfo());

    std::tstringstream sql;
    sql << _T("SELECT tItems.keylow AS itemloid, tItems.keyhigh AS itemhiid, tItems.ql AS itemql, name AS itemname, ")
        << _T("(SELECT tToons.charname FROM tToons WHERE tToons.charid = owner) AS ownername, owner AS ownerid, ")
        << _T("parent AS containerid ")
        << _T("FROM tItems JOIN tblAO ON keylow = aoid WHERE itemidx = ") << (int)itemID;

    SQLite::TablePtr pT = ExecTable(sql.str());

    pRetVal->itemloid = from_ascii_copy(pT->Data(0, 0));
    pRetVal->itemhiid = from_ascii_copy(pT->Data(0, 1));
    pRetVal->itemql = from_ascii_copy(pT->Data(0, 2));
    pRetVal->itemname = from_ascii_copy(pT->Data(0, 3));
    pRetVal->ownername = from_ascii_copy(pT->Data(0, 4));
    pRetVal->ownerid = from_ascii_copy(pT->Data(0, 5));
    pRetVal->containerid = from_ascii_copy(pT->Data(0, 6));
    unsigned int containerid = boost::lexical_cast<unsigned int>(pRetVal->containerid);
    unsigned int ownerid = boost::lexical_cast<unsigned int>(pRetVal->ownerid);
    pRetVal->containername = ServicesSingleton::Instance()->GetContainerName(ownerid, containerid);

    return pRetVal;
}


unsigned int DBManager::GetAODBSchemeVersion(std::tstring const& filename) const
{
    unsigned int retval = 0;
    SQLite::Db db;

    if (db.Init(filename)) {
        try {
            SQLite::TablePtr pT = db.ExecTable(_T("SELECT Version FROM vSchemeVersion"));
            retval = boost::lexical_cast<unsigned int>(pT->Data(0,0));
        }
        catch(Db::QueryFailedException &/*e*/) {
            retval = 0;
        }
        catch (boost::bad_lexical_cast &/*e*/) {
            retval = 0;
        }
    }

    return retval;
}


unsigned int DBManager::GetDBVersion() const
{
    unsigned int retval = 0;

    try {
        SQLite::TablePtr pT = ExecTable(_T("SELECT Version FROM vSchemeVersion"));
        retval = boost::lexical_cast<unsigned int>(pT->Data(0,0));
    }
    catch(Db::QueryFailedException &/*e*/) {
        retval = 0;
    }
    catch (boost::bad_lexical_cast &/*e*/) {
        retval = 0;
    }

    return retval;
}


void DBManager::UpdateDBVersion(unsigned int fromVersion) const
{
    switch (fromVersion)
    {
    case 0:
        {
            Begin();
            Exec(_T("CREATE TABLE tToons2 (charid, charname)"));
            Exec(_T("INSERT INTO tToons2 (charid, charname) SELECT charid, charname FROM tToons"));
            Exec(_T("DROP TABLE tToons"));
            Exec(_T("CREATE TABLE tToons (charid, charname)"));
            Exec(_T("CREATE UNIQUE INDEX iCharId ON tToons (charid)"));
            Exec(_T("INSERT INTO tToons (charid, charname) SELECT charid, charname FROM tToons2"));
            Exec(_T("DROP TABLE tToons2"));
            Exec(_T("CREATE VIEW vSchemeVersion AS SELECT '1' AS Version"));
            Commit();
        }
        // Dropthrough

    case 1:
        {

        }
        // Dropthrough

    default:
        break;
    }
}


void DBManager::CreateDBScheme() const
{
    Begin();
    Exec(_T("CREATE TABLE tItems (itemidx INTEGER NOT NULL PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT UNIQUE DEFAULT '1', keylow INTEGER, keyhigh INTEGER, ql INTEGER, stack INTEGER DEFAULT '1', parent INTEGER NOT NULL DEFAULT '2', slot INTEGER, children INTEGER, owner INTEGER NOT NULL)"));
    Exec(_T("CREATE VIEW vBankItems AS SELECT * FROM tItems WHERE parent=1"));
    Exec(_T("CREATE VIEW vContainers AS SELECT * FROM tItems WHERE children > 0"));
    Exec(_T("CREATE VIEW vInvItems AS SELECT * FROM tItems WHERE parent=2"));
    Exec(_T("CREATE INDEX iOwner ON tItems (owner)"));
    Exec(_T("CREATE INDEX iParent ON tItems (parent)"));
    Exec(_T("CREATE TABLE tToons (charid, charname)"));
    Exec(_T("CREATE UNIQUE INDEX iCharId ON tToons (charid)"));
    Exec(_T("CREATE VIEW vSchemeVersion AS SELECT '1' AS Version"));
    Commit();
}
