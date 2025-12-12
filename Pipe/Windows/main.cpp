#include <windows.h>
#include <iostream>
#include <cstring>

BOOL CreateChildProcess(LPCSTR exeName, HANDLE hStdInput, HANDLE hStdOutput, PROCESS_INFORMATION* pi) {
    STARTUPINFOA si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdInput;
    si.hStdOutput = hStdOutput;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    char cmdLine[MAX_PATH];
    strcpy_s(cmdLine, exeName);

    return CreateProcessA(
        NULL,
        cmdLine,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        pi
    );
}

int main() {
    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    
    HANDLE hRead1, hWrite1;
    HANDLE hRead2, hWrite2;  
    HANDLE hRead3, hWrite3;  
    HANDLE hRead4, hWrite4;  

    if (!CreatePipe(&hRead1, &hWrite1, &sa, 0) ||
        !CreatePipe(&hRead2, &hWrite2, &sa, 0) ||
        !CreatePipe(&hRead3, &hWrite3, &sa, 0) ||
        !CreatePipe(&hRead4, &hWrite4, &sa, 0)) {
        std::cerr << "Failed to create pipes" << std::endl;
        return 1;
    }

    PROCESS_INFORMATION piM, piA, piP, piS;

    if (!CreateChildProcess("M.exe", hRead1, hWrite2, &piM)) {
        std::cerr << "Failed to create M process" << std::endl;
        return 1;
    }
    CloseHandle(hRead1);
    CloseHandle(hWrite2);

    if (!CreateChildProcess("A.exe", hRead2, hWrite3, &piA)) {
        std::cerr << "Failed to create A process" << std::endl;
        return 1;
    }
    CloseHandle(hRead2);
    CloseHandle(hWrite3);


    if (!CreateChildProcess("P.exe", hRead3, hWrite4, &piP)) {
        std::cerr << "Failed to create P process" << std::endl;
        return 1;
    }
    CloseHandle(hRead3);
    CloseHandle(hWrite4);

    if (!CreateChildProcess("S.exe", hRead4, GetStdHandle(STD_OUTPUT_HANDLE), &piS)) {
        std::cerr << "Failed to create S process" << std::endl;
        return 1;
    }
    CloseHandle(hRead4);

    const char* data = "1 2 3 4 5 6 7 8 9 10\n";
    DWORD bytesWritten;
    if (!WriteFile(hWrite1, data, (DWORD)strlen(data), &bytesWritten, NULL)) {
        std::cerr << "Failed to write to pipe" << std::endl;
    }
    CloseHandle(hWrite1);

    HANDLE hProcesses[4] = {piM.hProcess, piA.hProcess, piP.hProcess, piS.hProcess};
    WaitForMultipleObjects(4, hProcesses, TRUE, INFINITE);


    for (int i = 0; i < 4; i++) {
        CloseHandle(hProcesses[i]);
    }
    CloseHandle(piM.hThread);
    CloseHandle(piA.hThread);
    CloseHandle(piP.hThread);
    CloseHandle(piS.hThread);

    return 0;
}