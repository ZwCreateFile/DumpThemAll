#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include "prosecc.h"

void ListProcessModules2(DWORD dwPID)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "CreateToolhelp32Snapshot failed" << std::endl;
        return;
    }

    
    me32.dwSize = sizeof(MODULEENTRY32);

    
    if (!Module32First(hModuleSnap, &me32))
    {
        std::cout << "Module32First failed" << std::endl;
        CloseHandle(hModuleSnap);
        return;
    }

    
    do
    {
        std::wcout << me32.szModule << std::endl;
    } while (Module32Next(hModuleSnap, &me32));

    
    CloseHandle(hModuleSnap);
}

int process2()
{
    
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        std::cout << "EnumProcesses failed" << std::endl;
        return 1;
    }

    
    cProcesses = cbNeeded / sizeof(DWORD);

    
    for (unsigned int i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            
            if (NULL != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                }
            }

            
            std::wcout << szProcessName << " (PID: " << aProcesses[i] << ")" << std::endl;

            
            CloseHandle(hProcess);
        }
    }

    
    DWORD dwPID;
    std::wcout << "Enter PID: ";
    std::wcin >> dwPID;

    
    ListProcessModules2(dwPID);

    return 0;
}