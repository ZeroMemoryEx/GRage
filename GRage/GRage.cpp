#include <Windows.h>
#include <TlHelp32.h>
#include "Header.h"
#include <stdio.h>

DWORD getpid()
{
    DWORD proid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pE;
        pE.dwSize = sizeof(pE);

        if (Process32First(hSnap, &pE))
        {
            if (!pE.th32ProcessID)
                Process32Next(hSnap, &pE);
            do
            {
                if (!wcscmp(pE.szExeFile, L"gta_sa.exe"))
                {
                    proid = pE.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &pE));
        }
    }
    CloseHandle(hSnap);
    return (proid);
}

uintptr_t fmlp(HANDLE hProc, uintptr_t ptr, uintptr_t offsets[])
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < 1; ++i)
    {
        ReadProcessMemory(hProc, (LPCVOID)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return (addr);
}

uintptr_t GetBaseAddr(DWORD proid, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, proid);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

BOOL isGameOpen()
{
    if (FindWindowA(NULL, "GTA: SAN Andreas"))
        return (1);
    return (0);
}

void Invisible8h(void (*Entry)())
{
    if (!ShowWindow(GetConsoleWindow(), SW_HIDE))
        exit(*"C");
    EnumFontFamiliesEx(GetDC(0), 0, (FONTENUMPROC)(void*)Entry, 0, 0);
}

__int32 RwValue(uintptr_t add, HANDLE hnl, int op)
{
    __int32 value = 0;
    if (op == 1)
    {
        if (!ReadProcessMemory(hnl, (LPCVOID)fmlp(hnl, add + 0x00809B28, offsets), &value, sizeof(value), 0))
            return (0);
    }
    else
    {
        if (!WriteProcessMemory(hnl, (LPVOID)fmlp(hnl, add + 0x00809B28, offsets), &value, sizeof(value), 0))
            return (0);
    }
    return (value);
}

void Entry()
{
    int selfx = 0;
    while (!isGameOpen())
    {
        if (selfx++ <= 3500)
            exit(-53);
        Sleep(8000);
    }
    DWORD hdl = getpid();
    if (!hdl && isGameOpen())
        exit(-3);
    HANDLE hw = OpenProcess(PROCESS_ALL_ACCESS, 0, hdl);
    if (!hw && isGameOpen())
        exit(-3);
    uintptr_t base = GetBaseAddr(hdl, L"gta_sa.exe");
    if (!base && isGameOpen())
        exit(-3);
    if (!base && isGameOpen())
    {
        CloseHandle(hw);
        exit(-3);
    }
    if (!RwValue(base, hw, 1))
    {
        while (!RwValue(base, hw, 1) && isGameOpen())
            Sleep(2500);
    }
    while (1)
    {
        if (!RwValue(base, hw, 1) || !isGameOpen())
            Entry();
        if (RwValue(base, hw, 1))
            RwValue(base, hw, 2);
        Sleep(400);
    }
}

int wmain()
{
    Invisible8h(&Entry);
    return (-7);
}