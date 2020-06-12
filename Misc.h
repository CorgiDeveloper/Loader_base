#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <sstream>

#include <windows.h>
#include <winnt.h>
#include <winerror.h>
#include <minwindef.h>
#include <securitybaseapi.h>
#include <WinBase.h>
#include <consoleapi2.h>
#include <stdlib.h>
#include <TlHelp32.h>

#include <curl/curl.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <TlHelp32.h>

BOOL Admin()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}

void CheckAdmin()
{
    if (Admin() == TRUE)
    {

    }
    else
    {
        MessageBoxA(NULL, "Run as administrator.", "Authentication", MB_OK);
        exit(1);
    }
}
void CursorFlag(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}
void RandomTitle()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> distr(0, 51);
    std::string name = "";
    char alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
    for (int i = 0; i < 55; ++i)
    {
        name = name + alphabet[distr(mt)];
        SetConsoleTitleA(name.c_str());
    }
}