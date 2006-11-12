// dbtest.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dllmain.h"
#include <shared/aopackets.h>
#include <madCodeHookLib/Dll/MadCodeHookLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>


typedef void Message_t;

Message_t* ( * pOriginalDataBlockToMessage )( int _Size, void* _pDataBlock );
void LoadMessageFilter(HKEY hKeyParent, LPCTSTR lpszKeyName);

//Db db(NULL, 0);
int g_counter = -1;
std::set<unsigned int> g_messageFilter;

using namespace AO;


Message_t* DataBlockToMessageHook( int _Size, void* _pDataBlock )
{
	if (++g_counter > 10)
	{
		g_counter = 0;
	}

	if (g_counter == 0)
	{
		LoadMessageFilter(HKEY_CURRENT_USER, "Software\\AOMessageHook\\MessageIDs");
	}

	Header * msg = (Header*)_pDataBlock;

	unsigned int msgId = _byteswap_ulong(msg->msgid);

	if (g_messageFilter.find(msgId) != g_messageFilter.end() || g_messageFilter.size() == 0)
	{
		HWND hWnd;
		if( hWnd = FindWindow ( "ItemAssistantWindowClass", NULL ) ) // TODO make this a list in registry
		{
			COPYDATASTRUCT Data;
			Data.cbData = _Size;
			Data.lpData = _pDataBlock;

			SendMessage( hWnd, WM_COPYDATA, 0, ( LPARAM )&Data );
		}
	}

	return pOriginalDataBlockToMessage( _Size, _pDataBlock );
}


void LoadMessageFilter(HKEY hKeyParent, LPCTSTR lpszKeyName)
{
	g_messageFilter.empty();

	ATL::CRegKey reg;
	if (reg.Open(hKeyParent, lpszKeyName, KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR subkey[256];
		DWORD skLength = 256;
		DWORD dw;
		int index = 0;

		while (true)
		{
			if (reg.EnumKey(index, subkey, &skLength) == ERROR_SUCCESS)
			{
				index++;
				if (reg.QueryDWORDValue(subkey, dw) == ERROR_SUCCESS)
				{
					g_messageFilter.insert(dw);
				}
			}
			else
			{
				break;
			}
		}
	}
}


int ProcessAttach( HINSTANCE _hModule )
{
	pOriginalDataBlockToMessage = NULL;

	// Hook DataBlockToMessage
	HookAPI( "MessageProtocol.dll", "?DataBlockToMessage@@YAPAVMessage_t@@IPAX@Z", 
		DataBlockToMessageHook, ( void** )&pOriginalDataBlockToMessage );

	FlushInstructionCache( GetCurrentProcess(), NULL, 0 );

	return TRUE;
}


int ProcessDetach( HINSTANCE _hModule )
{
	if( pOriginalDataBlockToMessage )
		UnhookCode( ( void** )&pOriginalDataBlockToMessage );

	//db.close(0);

	return TRUE;
}


BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		return ProcessAttach( hModule );
	case DLL_PROCESS_DETACH:
        return ProcessDetach( hModule );
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}
