#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

class Mem {
private:
public:
    HANDLE handle;
    inline static uintptr_t procID;

    ~Mem() {                                                                 //destructor
        CloseHandle(handle);
    }

    uintptr_t getProcessID(const char* proc) {
        HANDLE hProcessId = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // current process
        uintptr_t procID;
        PROCESSENTRY32 pEntry; // process list
        pEntry.dwSize = sizeof(pEntry);

        do {
            if (!strcmp(pEntry.szExeFile, proc)) {
                procID = pEntry.th32ProcessID;
                CloseHandle(hProcessId);
                handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, procID);
                return this->procID = procID;
            }

        } while (Process32Next(hProcessId, &pEntry)); //gets next processes

        return NULL;
    }

    uintptr_t getModule(uintptr_t procID, const char* modName) {
        HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
        MODULEENTRY32 mEntry;
        mEntry.dwSize = sizeof(mEntry);

        do {
            if (!strcmp(mEntry.szModule, modName)) {
                CloseHandle(hModule);
                
                return (uintptr_t)mEntry.hModule;
            }
        } while (Module32Next(hModule, &mEntry));

        return NULL;
    }

    uintptr_t getAddress(uintptr_t addr, std::vector<uintptr_t> vect) {
        for (unsigned int i = 0; i < vect.size(); i++) {
            Toolhelp32ReadProcessMemory(procID, (BYTE*)addr, &addr, sizeof(addr), 0);
            addr += vect[i];
        }
        return addr;
    }

    template <class val>
    void writeMem(uintptr_t addr, val value) {
        WriteProcessMemory(handle, (LPBYTE*)addr, &value, sizeof(value), NULL);
    }

    int readMemInt(uintptr_t addr) {
        int x;
        Toolhelp32ReadProcessMemory(procID, (LPBYTE*)addr, &x, sizeof(x), NULL);
        return x;
    }
    float readMemFloat(uintptr_t addr) {
        float x;
        Toolhelp32ReadProcessMemory(procID, (LPBYTE*)addr, &x, sizeof(x), NULL);
        return x;
    }

    void readString(uintptr_t addr, char* buffer, SIZE_T size) {
        Toolhelp32ReadProcessMemory(procID, (LPBYTE*)addr, buffer, size, NULL);
    }
};
