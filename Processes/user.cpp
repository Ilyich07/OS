#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>

DWORD getProcessIdByName(const char* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0; 
    }

    DWORD pid = 0;

    if (Process32First(snapshot, &entry)) {
        do {
            #ifdef UNICODE
            char exeFile[MAX_PATH];
            WideCharToMultiByte(CP_ACP, 0, entry.szExeFile, -1, exeFile, MAX_PATH, NULL, NULL);
            #else
            const char* exeFile = entry.szExeFile;
            #endif

            if (_stricmp(exeFile, processName) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return pid;
}

bool isProcessRunning(DWORD pid) {
    if (pid == 0) return false;
    HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (handle != NULL) {
        DWORD exitCode;
        if (GetExitCodeProcess(handle, &exitCode)) {
            CloseHandle(handle);
            return (exitCode == STILL_ACTIVE);
        }
        CloseHandle(handle);
    }
    return false;
}

bool isProcessRunning(const char* processName) {
    DWORD id = getProcessIdByName(processName);
    bool result = isProcessRunning(id);
    return result;
}

bool runKiller(const char* option, const char* value) {
    std::string command = "killer.exe " + std::string(option) + " " + std::string(value);
    
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    if (CreateProcessA(
        NULL,                      
        const_cast<char*>(command.c_str()), 
        NULL,                   
        NULL,                         
        FALSE,                        
        0,                           
        NULL,                   
        NULL,                           
        &si,                           
        &pi                             
    )) {

        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        return (exitCode == 0);
    }
    
    return false;
}

void checkProcesses(const char* option, const char* value) {
    char buffer[256];
    if (GetEnvironmentVariableA("PROC_TO_KILL", buffer, sizeof(buffer)) > 0) {
        std::stringstream ss(buffer);
        std::string processNameInVar;
        std::cout << "Processes from PROC_TO_KILL:" << std::endl;
        while (std::getline(ss, processNameInVar, ',')) {
            if (!processNameInVar.empty()) {
                std::cout << "  " << processNameInVar << (isProcessRunning(processNameInVar.c_str()) ? " RUNNING" : " NOT RUNNING") << std::endl;
            }   
        }
    } else {
        std::cout << "PROC_TO_KILL environment variable not set" << std::endl;
    }

    if (strcmp(option, "--name") == 0 && value != nullptr) {
        std::cout << "Process to kill by name: " << value 
                  << (isProcessRunning(value) ? " RUNNING" : " NOT RUNNING") << std::endl;
    } else if (strcmp(option, "--id") == 0 && value != nullptr) {
        DWORD pid = atoi(value);
        std::cout << "Process to kill by ID: " << pid 
                  << (isProcessRunning(pid) ? " RUNNING" : " NOT RUNNING") << std::endl;
    }
}

void testKillingProcesses(const char* option, const char* value) {
    std::cout << "\n====================" << std::endl;
    std::cout << "Testing Killer with " << option << " " << value << std::endl;
    std::cout << "--------------------" << std::endl;
    
    checkProcesses(option, value);
    
    std::cout << "\nRunning Killer..." << std::endl;
    if (runKiller(option, value)) {
        std::cout << "Killer executed successfully" << std::endl;
        Sleep(2000);
        std::cout << "\nAfter Killer execution:" << std::endl;
        checkProcesses(option, value);
    } else {
        std::cout << "Killer failed" << std::endl;
    }
}

void launchProcess(const char* processName) {
    std::string command = "start " + std::string(processName);
    system(command.c_str());
    std::cout << "Launched: " << processName << std::endl;
    Sleep(1000);
}

int main() {
    const char* processesToKill = "notepad.exe,calc.exe,mspaint.exe";
    SetEnvironmentVariableA("PROC_TO_KILL", processesToKill);
    std::cout << "Set PROC_TO_KILL = " << processesToKill << std::endl;

    std::cout << "\nLaunching test processes..." << std::endl;
    launchProcess("notepad.exe");
    launchProcess("calc.exe");
    launchProcess("mspaint.exe");


    Sleep(2000);


    testKillingProcesses("--name", "calc.exe");

    launchProcess("notepad.exe");
    launchProcess("mspaint.exe");
    Sleep(2000);

    DWORD notepadPid = getProcessIdByName("notepad.exe");
    if (notepadPid > 0) {
        char pidStr[20];
        sprintf(pidStr, "%lu", notepadPid);
        testKillingProcesses("--id", pidStr);
    }

    SetEnvironmentVariableA("PROC_TO_KILL", NULL);
    return 0;
}