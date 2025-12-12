#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>

void killProcessesFromEnvironment();
void killProcessByPID(const char* pid);
void killProcessByName(const char* name);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: killer.exe --id PID   OR   killer.exe --name NAME\n";
        return 1;
    }

   
    killProcessesFromEnvironment();

    if (strcmp(argv[1], "--id") == 0) {
        killProcessByPID(argv[2]);
    } else if (strcmp(argv[1], "--name") == 0) {
        killProcessByName(argv[2]);
    } else {
        std::cerr << "Unknown option: " << argv[1] << "\n";
        std::cerr << "Usage: killer.exe --id PID   OR   killer.exe --name NAME\n";
        return 2;
    }

    return 0;
}

void killProcessByPID(const char* pid) {
    DWORD dwPid = atoi(pid);
    if (dwPid == 0) {
        std::cout << "Invalid PID: " << pid << std::endl;
        return;
    }
    
    std::cout << "Killing process by PID: " << dwPid << std::endl;
    HANDLE handleOfProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);
    if (handleOfProcess != NULL) {
        if (TerminateProcess(handleOfProcess, 0)) {
            std::cout << "  Successfully killed PID " << dwPid << std::endl;
        } else {
            std::cout << "  Failed to kill PID " << std::endl;
        }
        CloseHandle(handleOfProcess);
    } else {
        std::cout << "  Process with PID " << dwPid << " not found or access denied" << std::endl;
    }
}

void killProcessByName(const char* name) {
    std::cout << "Killing process by name: " << name << std::endl;
    
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    int killedCount = 0;
    if (Process32First(snapshot, &entry)) {
        do {
            #ifdef UNICODE
            char exeFileName[MAX_PATH];
            WideCharToMultiByte(CP_ACP, 0, entry.szExeFile, -1, exeFileName, MAX_PATH, NULL, NULL);
            #else
            char* exeFileName = entry.szExeFile;
            #endif

            if (_stricmp(exeFileName, name) == 0) {
                HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (handle != NULL) {
                    if (TerminateProcess(handle, 0)) {
                        std::cout << "  Successfully killed " << name << std::endl;
                        killedCount++;
                    }
                    CloseHandle(handle);
                }
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);

}

void killProcessesFromEnvironment() {
    char envirVal[MAX_PATH];
    DWORD result = GetEnvironmentVariableA("PROC_TO_KILL", envirVal, sizeof(envirVal));

    if (result == 0) {
        std::cout << "PROC_TO_KILL environment variable not found" << std::endl;
        return;
    }

    if (result > sizeof(envirVal)) {
        std::cout << "Environment variable too long" << std::endl;
        return;
    }

    std::cout << "PROC_TO_KILL = " << envirVal << std::endl;
    
    std::vector<std::string> processNames;
    std::stringstream stream(envirVal);
    std::string name;

    while (std::getline(stream, name, ',')) {
        // Удаляем пробелы в начале и конце
        size_t start = name.find_first_not_of(" \t");
        size_t end = name.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos) {
            processNames.push_back(name.substr(start, end - start + 1));
        }
    }

    for (const std::string& procName : processNames) { 
        if (!procName.empty()) {
            killProcessByName(procName.c_str());
        }
    }
}