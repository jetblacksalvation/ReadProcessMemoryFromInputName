#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>
#include <iostream>

#include <string>

#include <algorithm>
#include <iterator>



template <class InIter1, class InIter2, class OutIter>
void copy_into_iter(InIter1 buf_start, InIter1 buf_end, InIter2 pat_start, InIter2 pat_end, OutIter res) {
        
}
void readMempages(HANDLE hProcess);
void getAllHandles() {

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        exit(1);
    }
    cProcesses = cbNeeded / sizeof(DWORD);//count processes...

    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

            // Get a handle to the process.

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE, aProcesses[i]);

            // Get the process name.

            if (NULL != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                    &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, szProcessName,
                        sizeof(szProcessName) / sizeof(TCHAR));
                }
            }

            // Print the process name and identifier.
#ifndef UNICODE
            auto str = szProcessName;
#else
            std::wstring wStr = szProcessName;
            auto str = std::string(wStr.begin(), wStr.end());
#endif
            //std::cout << str << "is processname\n";
            
            if (str.find(std::string("chrome")) != std::string::npos) {
                std::cout << "chrome is true!\n";
                readMempages(hProcess);
            }
            //else {
            //    std::cout << str.c_str() << '\n';
            //}
            //_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, aProcesses[i]);
        }

    }
}
void readMempages(HANDLE hProcess) {
    LPCVOID AddrPointer = new LPCVOID;
    MEMORY_BASIC_INFORMATION BasicMemptr;
    unsigned char* p = NULL;
    for (p = NULL;
        VirtualQueryEx(hProcess, AddrPointer, &BasicMemptr, sizeof(BasicMemptr)) == sizeof(BasicMemptr);
        p += BasicMemptr.RegionSize) {
        std::vector<char> buffer;
        if (BasicMemptr.State == MEM_COMMIT &&
            (BasicMemptr.Type == MEM_MAPPED || BasicMemptr.Type == MEM_PRIVATE)) {
            SIZE_T bytes_read;
            buffer.resize(BasicMemptr.RegionSize);
            ReadProcessMemory(hProcess, p, &buffer[0], BasicMemptr.RegionSize, &bytes_read);
            std::cout << buffer.data() << '\n';
            //find_all(p, buffer.begin(), buffer.end(), pattern.begin(), pattern.end(), std::ostream_iterator<void*>(std::cout, "\n"));
            //<<---- some of this is stolen code, i wrote the process iteration code
            std::cout << "finished one iteration\n";
        }
    }
    CloseHandle(hProcess);

}
int main(void)
{
    getAllHandles();
}
