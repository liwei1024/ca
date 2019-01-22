#include "Kca.h"
#include <direct.h>
#include <string>


#ifndef DRIVER_CONTROL_H
#include "../driver_control/drictl.h"
#pragma comment(lib,"driver_control.lib")
#endif // !DRIVER_CONTROL_H

#include "../VMProtectSDK.h"

LPCWSTR g_SymboliLinkName = L"";
LPCWSTR g_DeviceShortName = L"";

Kca::Kca()
{
	Init();
}

Kca::~Kca()
{
	//closeHandle();
}

void Kca::Init()
{
	
	wchar_t buffer[MAX_PATH];
	_wgetcwd(buffer, MAX_PATH);
	std::wstring driverFilePath(buffer);
	driverFilePath += VMProtectDecryptStringW(L"\\sys\\" DRIVER_FILE_NAME);

	g_SymboliLinkName = VMProtectDecryptStringW(L"\\\\.\\" SYMBOLIC_LINK_SHORT_NAME);
	g_DeviceShortName = VMProtectDecryptStringW(DEVICE_SHOST_NAME);

	/*printf("g_SymboliLinkName->:%ws\n", g_SymboliLinkName);
	printf("g_DeviceShortName->:%ws\n", g_DeviceShortName);
	printf("driverFilePath->:%ws\n", driverFilePath.c_str());*/

	if (drictl::install(g_SymboliLinkName, g_DeviceShortName, driverFilePath.c_str())) {
		//dwProcessId = getProcessId();
		//modifyCurrentProcessPath((ULONG)getProcessIdByProcessName(L"svchost.exe"));
		protectCurrentProcess(TRUE);
		//setProtection(GetCurrentProcessId(), Policy_Disable);
		//protectCurrentProcessFile();
		//getModuleHandleByModuleName(L"tcj.dll");
	}
}

void Kca::closeHandle()
{
	dwProcessId = 0;
	//drictl::uninstall(g_DeviceShortName);
	//dwProcessBaseAddress = 0;
	protectCurrentProcess(FALSE);
}

//BOOL Kca::setProtection(
//	DWORD pid,
//	PolicyOpt protection,
//	PolicyOpt dynamicCode /*= Policy_Keep*/,
//	PolicyOpt binarySignature /*= Policy_Keep*/
//)
//{
//	SET_PROC_PROTECTION setProt = { pid, protection, dynamicCode, binarySignature };
//	return drictl::control(g_SymboliLinkName, KCA_SET_PROTECTION, &setProt, sizeof(setProt), 0, 0);
//}

ULONG Kca::getProcessId()
{
	ULONG processId;
	drictl::control(g_SymboliLinkName,KCA_GET_PROCESS_ID, &processId, sizeof(processId), &processId, sizeof(processId));
	return processId;
}

BOOL Kca::protectCurrentProcess(BOOLEAN enable)
{
	return drictl::control(g_SymboliLinkName, KCA_PROTECT_CURRENT_PROCESS, &enable, sizeof(enable),0, 0);
}

BOOL Kca::modifyCurrentProcessPath(ULONG ProcessId)
{
	return drictl::control(g_SymboliLinkName, KCA_MODIFY_CURRENT_PROCESS_PATH, &ProcessId, sizeof(ProcessId), 0, 0);
}

BOOL Kca::protectCurrentProcessFile() 
{
	return drictl::control(g_SymboliLinkName, KCA_PROTECT_CURRENT_PROCESS_FILE, 0, 0, 0, 0);
}

BOOL Kca::unProtectCurrentProcessFile()
{
	return drictl::control(g_SymboliLinkName, KCA_UN_PROTECT_CURRENT_PROCESS_FILE, 0, 0, 0, 0);
}

HANDLE Kca::getProcessHandle()
{
	if (hProcess == NULL || dwProcessId != getProcessId())
	{
		dwProcessId = getProcessId();
		drictl::control(g_SymboliLinkName,KCA_GET_PROCESS_HANDLE, &hProcess, sizeof(hProcess), &hProcess, sizeof(hProcess));
		CloseHandle(hProcess);
	}
	return hProcess;
}

BOOL Kca::readVirtualMemory(ULONG Address, PVOID Response, SIZE_T Size)
{
	KCA_READ_VIRTUAL_MEMORY_STRUCT rvms;
	rvms.Response = Response;
	rvms.Address = Address;
	rvms.Size = Size;
	return drictl::control(g_SymboliLinkName,KCA_READ_VIRTUAL_MEMORY, &rvms, sizeof(rvms), &rvms, sizeof(rvms));
}

BOOL Kca::writeVirtualMemory(ULONG Address, PVOID Value, SIZE_T Size)
{
	KCA_WRITE_VIRTUAL_MEMORY_STRUCT wvms;
	BOOL result = TRUE;
	wvms.Address = Address;
	wvms.Value = Value;
	wvms.Size = Size;
	result =  drictl::control(g_SymboliLinkName,KCA_WRITE_VIRTUAL_MEMORY, &wvms, sizeof(wvms), &wvms, sizeof(wvms));
	if (result == FALSE)
	{
		result = writeVirtualMemoryEx(Address, Value, Size);
	}
	return result;
}

BOOL Kca::writeVirtualMemoryEx(ULONG Address, PVOID Value, SIZE_T Size)
{
	DWORD oldProtect;
	BOOL result = TRUE;
	result = VirtualProtectEx(getProcessHandle(),(LPVOID)(ULONG_PTR)Address, Size,PAGE_EXECUTE_READWRITE, &oldProtect);
	/*if (result) {
		printf("修改内存属性成功\n");
	}
	else {
		printf("修改内存属性失败\n");
	}*/
	KCA_WRITE_VIRTUAL_MEMORY_STRUCT wvms;
	wvms.Address = Address;
	wvms.Value = Value;
	wvms.Size = Size;
	result =  drictl::control(g_SymboliLinkName, KCA_WRITE_VIRTUAL_MEMORY, &wvms, sizeof(wvms), &wvms, sizeof(wvms));
	result = VirtualProtectEx(getProcessHandle(), (LPVOID)(ULONG_PTR)Address, Size, oldProtect, &oldProtect);
	/*if (result) {
		printf("还原内存属性成功\n");
	}
	else {
		printf("还原内存属性失败\n");
	}*/
	return result;
}

HMODULE Kca::getModuleHandleByModuleName(const wchar_t *moduleName)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModulesEx(getProcessHandle(), hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_32BIT))
	{
		//setlocale(LC_CTYPE, "");
		for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			GetModuleFileNameEx(getProcessHandle(), hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));
			if (wcsstr(szModName, moduleName)) {
				/*printf("moduleName->:%ws hmodule->:%p", szModName, hMods[i]);
				printf("\n");*/
				return hMods[i];
			}
		}
	}
	return NULL;
}


BOOL Kca::getModuleInfoByModuleName(LPMODULEINFO ModuleInfo, const wchar_t *moduleName)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;

	if (EnumProcessModulesEx(getProcessHandle(), hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_32BIT))
	{
		setlocale(LC_CTYPE, "");
		for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			GetModuleFileNameEx(getProcessHandle(), hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));
			if (wcsstr(szModName, moduleName)) {
				printf("moduleName->:%ws hmodule->:%p", szModName, hMods[i]);
				printf("\n");
				return GetModuleInformation(getProcessHandle(), hMods[i], ModuleInfo, sizeof(MODULEINFO));
			}
		}
	}
	
	return FALSE;
}

DWORD Kca::getProcessIdByProcessName(const wchar_t * processName)
{
	//bool result = false;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot Error!\n");
		return 0;
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);
	while (bResult)
	{
		if (wcscmp(processName, pe32.szExeFile) == 0) {
			return pe32.th32ProcessID;
			break;
		}
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}