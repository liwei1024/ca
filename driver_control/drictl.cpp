/***************************************************************************************************
Module: DriverControl.c
Author: ZChameleon @ 2015
***************************************************************************************************/


#include <Windows.h>
#include "drictl.h"

#include "../VMProtectSDK.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//1 符号链接名
//2 设备名
//3 驱动文件路径 

BOOL drictl::install(LPCWSTR SymboliLinkName, LPCWSTR DeviceShortName, LPCWSTR DriverFilePath)
{
	HANDLE hDriverFile;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	DWORD dwRtn;

	hDriverFile = CreateFileW(
		SymboliLinkName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		0,
		0
	);
	if (hDriverFile == INVALID_HANDLE_VALUE)
	{
		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager == NULL)
		{
			printf(VMProtectDecryptStringA("OpenSCManager() Faild %d ! \n"), GetLastError());
			return FALSE;
		}
		schService = CreateServiceW(
			schSCManager,
			DeviceShortName,
			DeviceShortName,
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			DriverFilePath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
		);
		if (schService == NULL)
		{
			dwRtn = GetLastError();
			if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
			{
				printf(VMProtectDecryptStringA("由于其他原因创建服务失败 %d ! \n"), dwRtn);
				return FALSE;
			}
		}

		schService = OpenService(schSCManager, DeviceShortName, SERVICE_ALL_ACCESS);
		if (schService == NULL)
		{
			printf(VMProtectDecryptStringA("OpenService:%d"), GetLastError());
			CloseServiceHandle(schSCManager);
			return FALSE;
		}

		if (!StartService(schService, NULL, NULL))
		{
			dwRtn = GetLastError();
			if (dwRtn == ERROR_IO_PENDING)
			{
				//设备被挂住
				printf(VMProtectDecryptStringA("StartService() Faild ERROR_IO_PENDING ! \n"));
			}
			else if (dwRtn == ERROR_SERVICE_ALREADY_RUNNING) {
				printf(VMProtectDecryptStringA("StartService() Faild ERROR_SERVICE_ALREADY_RUNNING ! \n"));
			}
			/*else {
				printf(VMProtectDecryptStringA("服务已经启动 ! \n"));
			}*/
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return FALSE;
		}
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
	}
	CloseHandle(hDriverFile);
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL drictl::uninstall(LPCWSTR DeviceShortName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	SERVICE_STATUS ss;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
	{
		printf(VMProtectDecryptStringA("OpenSCManager:%d\n"), GetLastError());
		return FALSE;
	}

	schService = OpenService(schSCManager, DeviceShortName, SERVICE_ALL_ACCESS);
	if (schService == NULL)
	{
		printf(VMProtectDecryptStringA("OpenService:%d\n"), GetLastError());
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	if (!ControlService(schService, SERVICE_CONTROL_STOP, &ss))
	{
		printf(VMProtectDecryptStringA("ControlService:%d\n"), GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	if (!DeleteService(schService))
	{
		printf(VMProtectDecryptStringA("DeleteService:%d\n"), GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL drictl::control(LPCWSTR SymbolicLinkName, DWORD IoControlCode,
	PVOID InBuffer, DWORD InBufferSize, PVOID OutBuffer, DWORD OutBufferSize)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD Junk = 0;
	DWORD errorCode = 0;

	hDevice = CreateFileW(SymbolicLinkName, 0, FILE_SHARE_READ |
		FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf(VMProtectDecryptStringA("CreateFile:%d\n"), GetLastError());
		return FALSE;
	}

	if (!DeviceIoControl(hDevice, IoControlCode, InBuffer,
		InBufferSize, OutBuffer, OutBufferSize, &Junk, NULL))
	{
		errorCode = GetLastError();
		if (errorCode != 299)
		{
			printf(VMProtectDecryptStringA("DeviceIoControl:%d\n"), GetLastError());
		}
		CloseHandle(hDevice);
		return FALSE;
	}
	CloseHandle(hDevice);
	return TRUE;
}


//////////////////////////////////////////// End Of File ///////////////////////////////////////////