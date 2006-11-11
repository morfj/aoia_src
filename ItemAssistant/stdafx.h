// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WTL_USE_CSTRING

#include "resource.h"

#define _WIN32_WINNT 0x0501
#include <Windows.h>

#include <atlbase.h>        // Base ATL classes
#include <atlapp.h>

extern CAppModule _Module;  // Global _Module

#include <atlwin.h>         // ATL windowing classes
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <atlcrack.h>
//#include <atltypes.h>

#include <atlcoll.h>
#include "TabbingFramework/atlgdix.h"
#include "TabbingFramework/CustomTabCtrl.h"
#include "TabbingFramework/DotNetTabCtrl.h"
#include "TabbingFramework/TabbedMDI.h"

#include <stdio.h>

#include <boost/shared_ptr.hpp>

#include "UnicodeSupport.h"

#include "SQLite/sqlite3.h"

//#include "DBManager.h"
class DBManager;
extern DBManager g_DBManager; // Global DB manager

#include <shared/Singleton.h>
#include "SharedServices.h"
typedef Shared::Singleton<SharedServices> ServicesSingleton;

#ifdef UNICODE
#define STREAM2STR( streamdef ) \
   (((std::wostringstream&)(std::wostringstream().flush() << streamdef)).str())
#else
#define STREAM2STR( streamdef ) \
   (((std::ostringstream&)(std::ostringstream().flush() << streamdef)).str())
#endif
