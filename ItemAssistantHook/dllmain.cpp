// dbtest.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dllmain.h"
#include <shared/aopackets.h>
#include <Detours/detours.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <queue>


typedef void Message_t;

Message_t* ( * pOriginalDataBlockToMessage )( int _Size, void* _pDataBlock );
void LoadMessageFilter(HKEY hKeyParent, LPCTSTR lpszKeyName);

//Db db(NULL, 0);
int g_counter = -1;
std::set<unsigned int> g_messageFilter;
HANDLE g_hEvent;

typedef std::queue<COPYDATASTRUCT*> COPYDATASTRUCTQUEUE;

COPYDATASTRUCTQUEUE g_dataQueue;
CRITICAL_SECTION g_CritSection;

using namespace AO;


void WorkerThreadMethod(void*)
{
    while ((g_hEvent != NULL) && (WaitForSingleObject(g_hEvent,INFINITE) == WAIT_OBJECT_0))
    {
        CCritSecLock cs(g_CritSection,true);

        if (g_hEvent != NULL)
        {
            HWND hWnd;
            if( hWnd = FindWindow ( "ItemAssistantWindowClass", NULL ) ) // TODO make this a list in registry
            {
                while (!g_dataQueue.empty())
                {
                    COPYDATASTRUCT * pData = g_dataQueue.front();
                    SendMessage( hWnd, WM_COPYDATA, 0, ( LPARAM ) pData );
                    g_dataQueue.pop();

                    delete[] pData->lpData;
                    delete pData;
                }
            }
        }
        else
        {
            break;
        }
    }

    DeleteCriticalSection(&g_CritSection);
}


void StartWorkerThread()
{
    InitializeCriticalSection(&g_CritSection);
    g_hEvent = CreateEvent(NULL,FALSE,FALSE,"IA_Worker");

    _beginthread(WorkerThreadMethod,0,0);
}


void EndWorkerThread()
{
    CCritSecLock cs(g_CritSection,true);
    //      EnterCriticalSection(&g_CritSection);

    SetEvent(g_hEvent);
    CloseHandle(g_hEvent);
    g_hEvent = NULL;
}


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
        COPYDATASTRUCT * pData = new COPYDATASTRUCT;
        if (pData != NULL)
        {
            pData->dwData = 0;
            pData->cbData = _Size;
            pData->lpData = new unsigned char[_Size];//_pDataBlock;

            if (pData->lpData != NULL)
            {
                memcpy(pData->lpData, _pDataBlock, _Size);
                g_dataQueue.push(pData);
                SetEvent(g_hEvent);
            }
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
    // Hook DataBlockToMessage
    pOriginalDataBlockToMessage = (Message_t *(__cdecl*)(int,void*))::GetProcAddress(::GetModuleHandle("MessageProtocol.dll"), "?DataBlockToMessage@@YAPAVMessage_t@@IPAX@Z");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&pOriginalDataBlockToMessage, DataBlockToMessageHook);
    DetourTransactionCommit();

    StartWorkerThread();

    return TRUE;
}


int ProcessDetach( HINSTANCE _hModule )
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((PVOID*)&pOriginalDataBlockToMessage, DataBlockToMessageHook);
    DetourTransactionCommit();

    EndWorkerThread();

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
