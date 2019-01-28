#ifndef __KCY_H__
#define __KCY_H__
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#ifndef KCA_API_H
#include "../KernelCheatAssistant/kca_api.h"
#endif // !KCA_API_H

class Kca
{
private:
	//HANDLE hDriver = NULL;
public:
	HANDLE hProcess = NULL;
	Kca();
	~Kca();
	ULONG dwProcessId = NULL;
	//DWORD dwProcessBaseAddress;
	void Init();
	void closeHandle();
	/*BOOL setProtection(
		DWORD pid,
		PolicyOpt protection,
		PolicyOpt dynamicCode = Policy_Keep,
		PolicyOpt binarySignature = Policy_Keep
	);*/
	ULONG getProcessId();
	BOOL protectCurrentProcess(BOOLEAN enable);
	BOOL modifyCurrentProcessPath(ULONG ProcessId);
	BOOL protectCurrentProcessFile();
	BOOL unProtectCurrentProcessFile();
	HANDLE getProcessHandle();
	//DWORD getProcessBaseAddress();
	BOOL readVirtualMemory(ULONG Address, PVOID Response, SIZE_T Size);
	BOOL writeVirtualMemory(ULONG Address, PVOID Value, SIZE_T Size);
	BOOL writeVirtualMemoryEx(ULONG Address, PVOID Value, SIZE_T Size);
	HMODULE getModuleHandleByModuleName(const wchar_t * moduleName);
	BOOL getModuleInfoByModuleName(LPMODULEINFO ModuleInfo, const wchar_t * moduleName);
	DWORD getProcessIdByProcessName(const wchar_t * processName);
};
#endif // !__KCY_H__


