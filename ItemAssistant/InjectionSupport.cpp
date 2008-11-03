#include "stdafx.h"


BOOL InjectDLL(DWORD ProcessID, std::tstring const& dllName)
{
    Logger::instance().log(STREAM2STR("Attempting to inject " << dllName));

    std::string dllNameA = to_ascii_copy(dllName);


    if(!ProcessID) {
        Logger::instance().log(_T("No process ID specified to inject into."));
        return false;
    }

    HANDLE Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

    if(!Proc) {
        Logger::instance().log(STREAM2STR("Could not open process. Error code: " << GetLastError()));
        return false;
    }

    LPVOID LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
    LPVOID RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, dllNameA.length(), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (!RemoteString) {
        Logger::instance().log(STREAM2STR("Could not allocate memory in remote process. Error code: " << GetLastError()));
        return false;
    }
    WriteProcessMemory(Proc, (LPVOID)RemoteString, dllNameA.c_str(), dllNameA.length(), NULL);

    if(!CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL)) {
        DWORD error = GetLastError();
        Logger::instance().log(STREAM2STR("Injection failed with error code: " << error));
    }

    CloseHandle(Proc);

    return true;
} 
