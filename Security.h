#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include <sstream>

#include <minwindef.h>
#include <winreg.h>
#include <winerror.h>
#include <stdlib.h>
#include <memoryapi.h>
#include <sysinfoapi.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <handleapi.h>
#include <fileapi.h>
#include <errhandlingapi.h>
#include <TlHelp32.h>

#include <curl/curl.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#define Crash        \
    __asm{push eax}            \
    __asm{xor eax, eax}        \
    __asm{setpo al}            \
    __asm{push edx}            \
    __asm{xor edx, eax}        \
    __asm{sal edx, 2}        \
    __asm{xchg eax, edx}    \
    __asm{pop edx}            \
    __asm{or eax, ecx}        \
    __asm{pop eax}

void KillProcess(const char* filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}
void TerminateVPN()
{
	KillProcess("openvpn.exe");
}
void Webhook_Launched()
{
	TerminateVPN();
	Sleep(3000);
}
void Webhook_SecurityTrigger()
{
	TerminateVPN();
	Sleep(3000);
}

void Shutdown()
{
	Webhook_SecurityTrigger();
	Sleep(4000);
	Crash
}

BOOL VMwareCheck()
{
	BOOL bDetected = FALSE;

	__try
	{
		__asm
		{
			mov    ecx, 0Ah
			mov    eax, 'VMXh'
			mov    dx, 'VX'
			in    eax, dx
			cmp    ebx, 'VMXh'
			sete    al
			movzx   eax, al
			mov    bDetected, eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}

	return bDetected;
}
BOOL VMCheck()
{
	HKEY hKey;
	int i;
	char szBuffer[64];
	const char* szProducts[] = { "*VMWARE*", "*VBOX*", "*VIRTUAL*" };

	DWORD dwSize = sizeof(szBuffer) - 1;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Services\\Disk\\Enum", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, "0", NULL, NULL, (unsigned char*)szBuffer, &dwSize) == ERROR_SUCCESS)
		{
			for (i = 0; i < _countof(szProducts); i++)
			{
				if (strstr(szBuffer, szProducts[i]))
				{
					RegCloseKey(hKey);
					return TRUE;
				}
			}
		}

		RegCloseKey(hKey);
	}

	return FALSE;
}
BOOL SandboxieCheck()
{
	if (GetModuleHandle("SbieDll.dll") != NULL)
		return TRUE;


	return FALSE;
}
BOOL VirtualBoxCheck()
{
	BOOL bDetected = FALSE;

	if (LoadLibrary("VBoxHook.dll") != NULL)
		bDetected = TRUE;

	if (CreateFile("\\\\.\\VBoxMiniRdrDN", GENERIC_READ, \
		FILE_SHARE_READ, NULL, OPEN_EXISTING, \
		FILE_ATTRIBUTE_NORMAL, NULL) \
		!= INVALID_HANDLE_VALUE)
	{
		bDetected = TRUE;
	}

	return bDetected;
}
bool DebuggerCheck()
{
	unsigned char* pMem = NULL;
	SYSTEM_INFO sysinfo = { 0 };
	DWORD OldProtect = 0;
	void* pAllocation = NULL;

	GetSystemInfo(&sysinfo);

	pAllocation = VirtualAlloc(NULL, sysinfo.dwPageSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);

	if (pAllocation == NULL)
		return false;

	pMem = (unsigned char*)pAllocation;
	*pMem = 0xc3;


	if (VirtualProtect(pAllocation, sysinfo.dwPageSize,
		PAGE_EXECUTE_READWRITE | PAGE_GUARD,
		&OldProtect) == 0)
	{
		return false;
	}

	__try
	{
		__asm
		{
			mov eax, pAllocation
			push MemBpBeingDebugged
			jmp eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		VirtualFree(pAllocation, NULL, MEM_RELEASE);
		return false;
	}

	__asm {MemBpBeingDebugged:}
	VirtualFree(pAllocation, NULL, MEM_RELEASE);
	return true;
}
inline bool Int2DCheck()
{
	__try
	{
		__asm
		{
			int 0x2d
			xor eax, eax
			add eax, 2
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}
inline bool DebuggerPresentCheck()
{
	__try
	{
		__asm __emit 0xF3
		__asm __emit 0x64
		__asm __emit 0xF1
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

void CheckMain()
{
	if (VMwareCheck() == FALSE
		&& VirtualBoxCheck() == FALSE
		&& SandboxieCheck() == FALSE
		&& VMCheck() == FALSE
		&& Int2DCheck() == FALSE
		&& DebuggerCheck() == FALSE
		&& DebuggerPresentCheck() == FALSE);
    else
	{
		Shutdown();
    }
}