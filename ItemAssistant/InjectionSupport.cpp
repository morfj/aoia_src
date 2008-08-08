#include "stdafx.h"


BOOL InjectDLL(DWORD ProcessID, std::tstring const& dllName)
{
    std::string dllNameA = to_ascii_copy(dllName);

    HANDLE Proc;
    char buf[50]={0};
    LPVOID RemoteString, LoadLibAddy;

    if(!ProcessID)
        return false;

    Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

    if(!Proc) {
        //sprintf(buf, "OpenProcess() failed: %d", GetLastError());
        //MessageBox(NULL, buf, "Loader", NULL);
        return false;
    }

    LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");

    RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, dllNameA.length(), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(Proc, (LPVOID)RemoteString, dllNameA.c_str(), dllNameA.length(), NULL);
    if(!CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL)) {
        DWORD error = GetLastError();
    }

    CloseHandle(Proc);

    return true;
} 
