#include "stdafx.h"
#include "dllmain.h"
#include <shared/aopackets.h>
#include <Detours/detours.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <queue>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>


typedef void Message_t;

Message_t* ( * pOriginalDataBlockToMessage )( int _Size, void* _pDataBlock );
void LoadMessageFilter(HKEY hKeyParent, LPCTSTR lpszKeyName);

DWORD g_lastTick = 0;
DWORD g_lastThreadTick = 0;
std::set<unsigned int> g_messageFilter;
HANDLE g_hEvent;

typedef std::queue<COPYDATASTRUCT*> COPYDATASTRUCTQUEUE;

COPYDATASTRUCTQUEUE g_dataQueue;
boost::mutex g_dataQueueMutex;

HWND g_targetWnd = NULL;   // TODO: Make this a set/list that is dynamically updated

using namespace AO;


void WorkerThreadMethod(void*)
{
    while ((g_hEvent != NULL) && (WaitForSingleObject(g_hEvent,INFINITE) == WAIT_OBJECT_0))
    {
        if (g_hEvent == NULL) {
            break;
        }

        DWORD tick = GetTickCount();
        if (tick < g_lastThreadTick) {
            // Overflow
            g_lastThreadTick = tick;
        }

        if ((tick - g_lastThreadTick > 10000) || g_targetWnd == NULL) {
            // We either don't have a valid window target OR it has been more than 10 sec since we last update the target.
            g_targetWnd  = FindWindow ( "ItemAssistantWindowClass", NULL ); // TODO: make the class name a list in registry
            g_lastThreadTick = GetTickCount();
        }

        while (!g_dataQueue.empty())
        {
            COPYDATASTRUCT * pData = NULL;

            {   // Guard scope
                boost::lock_guard<boost::mutex> guard(g_dataQueueMutex);

                pData = g_dataQueue.front();
                g_dataQueue.pop();
            }

            if (g_targetWnd == NULL) {
                // We have data, but no target window, so just delete the message
                delete[] pData->lpData;
                delete pData;
                continue;
            }

            // To avoid blocking the main thread, we should not have a lock on the queue while we process the message.
            SendMessage( g_targetWnd, WM_COPYDATA, 0, ( LPARAM ) pData );

            delete[] pData->lpData;
            delete pData;
        }
    }
}


void StartWorkerThread()
{
    g_hEvent = CreateEvent(NULL,FALSE,FALSE,"IA_Worker");
    _beginthread(WorkerThreadMethod,0,0);
}


void EndWorkerThread()
{
    SetEvent(g_hEvent);
    CloseHandle(g_hEvent);
    g_hEvent = NULL;
}


Message_t* DataBlockToMessageHook( int _Size, void* _pDataBlock )
{
    // Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
    DWORD time = GetTickCount();

    if (time < g_lastTick) {
        // Overflow so just reset the g_lastTick
        g_lastTick = time;
    }
    if (time - g_lastTick > 10000) {
        // 10 sec since last update
        g_lastTick = time;
        LoadMessageFilter(HKEY_CURRENT_USER, "Software\\AOMessageHook\\MessageIDs");
    }

    Header * msg = (Header*)_pDataBlock;
    unsigned int msgId = _byteswap_ulong(msg->msgid);

    if (g_messageFilter.size() == 0 || g_messageFilter.find(msgId) != g_messageFilter.end())
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
                {
                    boost::lock_guard<boost::mutex> guard(g_dataQueueMutex);
                    g_dataQueue.push(pData);
                }
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
