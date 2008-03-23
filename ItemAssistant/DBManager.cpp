#include "StdAfx.h"
#include "dbmanager.h"
#include <shared/localdb.h>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace SQLite {


   Db::Db()
      : m_pDb(NULL)
   {
   }


   Db::~Db()
   {
      sqlite3_close(m_pDb);
   }


   bool Db::Init(std::tstring const& filename)
   {
      if (SQLITE_OK != sqlite3_open(to_ascii_copy(filename).c_str(), &m_pDb)) {
         sqlite3_close(m_pDb);
         m_pDb = NULL;
         return false;
      }
      return true;
   }


   void Db::Term()
   {
      if (m_pDb != NULL)
      {
         sqlite3_close(m_pDb);
         m_pDb = NULL;
      }
   }


   TablePtr Db::ExecTable(std::tstring const& sql) const
   {
      TablePtr pRes;
      char **result;
      int nrow;
      int ncol;

      std::string sql_utf8 = to_utf8_copy(sql);

      int retval = sqlite3_get_table(m_pDb, 
         sql_utf8.c_str(), 
         &result,          /* Result written to a char *[]  that this points to */
         &nrow,            /* Number of result rows written here */
         &ncol,            /* Number of result columns written here */
         NULL);

      if (SQLITE_OK == retval)
      {
         pRes = TablePtr(new Table(nrow, ncol, result));
      }
      else {
         assert(false);
         std::tstring msg = STREAM2STR("Query Failed with error code " << retval);
         throw QueryFailedException(msg);
      }

      sqlite3_free_table(result);
      return pRes;
   }


   bool Db::Exec(std::tstring const& sql) const
   {
      return (SQLITE_OK == sqlite3_exec(m_pDb, to_ascii_copy(sql).c_str(), NULL, NULL, NULL)) ? true : false;
   }


   void Db::Begin() const
   {
      Exec(_T("BEGIN TRANSACTION"));
   }


   void Db::Commit() const
   {
      Exec(_T("COMMIT TRANSACTION"));
   }


   void Db::Rollback() const
   {
      Exec(_T("ROLLBACK TRANSACTION"));
   }

}  // namespace SQLite


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
         if( fp = fopen( to_ascii_copy(AOExePath.str()).c_str(), "r" ) )
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
      FILE* fp;

      std::tstringstream pathOfExe;
      pathOfExe << AODir << _T("\\anarchy.exe");

      if( !( fp = fopen(to_ascii_copy(pathOfExe.str()).c_str(), "r") ) ) {
         MessageBox( NULL, _T("This is not AO's directory."), _T("ERROR"), MB_OK | MB_ICONERROR);
         return false;
      }
      fclose( fp );

      if (regKey.Open(HKEY_CURRENT_USER, _T("Software\\AOItemAssistant"), KEY_ALL_ACCESS) == ERROR_SUCCESS)
      {
         regKey.SetStringValue(_T("AOPath"), AODir.c_str());
         regKey.Close();
      }
   }

   m_aofolder = std::tstring(AODir);

   if (dbfile.empty())
   {
      dbfile = _T("ItemAssistant.db");
   }

   if (!SQLite::Db::Init(dbfile))
   {
      return false;
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

   Exec(_T("ATTACH DATABASE \"aoitems.db\" AS aodb"));

   return true;
}


void DBManager::Term()
{
   SQLite::Db::Term();
}


/* Prompt user for folder
(from AOMD)
*/
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


//void DBManager::SyncLocalItemDB(Db& itemDB, const char* AoDir)
//{
//	bool doUpdate = true;
//
//	/* Check if local database is up to date */
//	HANDLE hLocalDB = CreateFile( "AODatabase.bdb", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
//
//	std::tstring dbfile(AoDir);
//	dbfile += "\\cd_image\\data\\db\\ResourceDatabase.dat";
//	HANDLE hOrigDB = CreateFile( dbfile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
//
//	if( hLocalDB != INVALID_HANDLE_VALUE && hOrigDB != INVALID_HANDLE_VALUE )
//	{
//		FILETIME OrigTime, LocalTime;
//
//		GetFileTime( hLocalDB, NULL, NULL, &LocalTime );
//		GetFileTime( hOrigDB, NULL, NULL, &OrigTime );
//
//		if( CompareFileTime( &OrigTime, &LocalTime ) < 0 )
//		{
//			doUpdate = false;
//		}
//	}
//
//	if (hOrigDB != INVALID_HANDLE_VALUE)
//	{
//		CloseHandle( hOrigDB );
//	}
//	if (hLocalDB != INVALID_HANDLE_VALUE)
//	{
//		CloseHandle( hLocalDB );
//	}
//
//	if (doUpdate)
//	{
//		// DB out-of-date. Update it!
//		if( !CreateLocalDatabase(itemDB, AoDir) )
//		{
//			// Delete the database in case it was partly created,
//			// so we're not using a partial database on next
//			// execution.
//			DeleteFile( "AODatabase.bdb" );
//
//			::MessageBox( NULL, "Failed to create local database.", "ERROR", MB_OK | MB_ICONERROR );
//		}
//	}
//}


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
   unsigned int containerid = boost::lexical_cast<int>(pRetVal->containerid);
   unsigned int ownerid = boost::lexical_cast<int>(pRetVal->ownerid);
   pRetVal->containername = ServicesSingleton::Instance()->GetContainerName(ownerid, containerid);

   return pRetVal;
}


unsigned int DBManager::GetDBVersion() const
{
   unsigned int retval = 0;

   try {
      SQLite::TablePtr pT = ExecTable(_T("SELECT Version FROM vSchemeVersion"));
	   retval = boost::lexical_cast<unsigned int>(pT->Data(0,0));
   }
   catch(Db::QueryFailedException &e) {
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
