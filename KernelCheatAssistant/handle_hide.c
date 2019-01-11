#include "handle_hide.h"

//LIST_ENTRY g_ActiveProcessEntry;
//LIST_ENTRY g_SessionProcess;
#define DRIVER_TAG 0x30

PVOID g_KernelBase = NULL;
DYNAMIC_DATA	 g_dynData;
//SessionProcesslist方法——从会话进程列表中断开目标进程的链接
//SessionProcesslist Method - Unlink target process from session process list
NTSTATUS SessionProcessListHiding(PEPROCESS pep, BOOLEAN bHiding)
{
	if (!g_dynData.SessionProcessLinks)
		return STATUS_NOT_SUPPORTED;

	__try
	{
		//TODO: Add Rundown Protection, Critical Section/Region
		LIST_ENTRY *pLE = (LIST_ENTRY *)MAKEPTR(pep, g_dynData.SessionProcessLinks);

		if (bHiding)
		{
			pLE->Flink->Blink = pLE->Blink;
			pLE->Blink->Flink = pLE->Flink;

			//Avoid BSOD on process Rundown
			pLE->Flink = pLE;
			pLE->Blink = pLE;
		}
		else
		{
			//NULL
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_ACCESS_DENIED;
	}

	return STATUS_SUCCESS;
}
//ProcessHandleTable方法——删除目标进程的句柄表
//ProcessHandleTable Method - Remove handle table of target process
NTSTATUS RemoveProcessHandleTable(PEPROCESS pep)
{
	if (!g_dynData.ObjTable || !g_dynData.correctBuild || !g_dynData.ExRemoveHandleTable)
		return STATUS_NOT_SUPPORTED;

	__try
	{
		void *pHandleTable = *((void **)MAKEPTR(pep, g_dynData.ObjTable));
		fnExRemoveHandleTable ExRemoveHandleTable = (fnExRemoveHandleTable)((ULONG_PTR)GetKernelBase() + g_dynData.ExRemoveHandleTable);
		ExRemoveHandleTable(pHandleTable);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

		return STATUS_ACCESS_DENIED;
	}

	return STATUS_SUCCESS;
}
//PspCidTable方法——销毁目标进程和该进程的线程的句柄
//PspCidTable Method - Destroy handle for target process and threads of that process
NTSTATUS RemovePspCidTable(PEPROCESS pep, HANDLE pid)
{
	LIST_ENTRY *ThreadListHead;
	LIST_ENTRY *pLE;
	fnExExDestroyHandle ExDestroyHandle;

	if (!g_dynData.UniqueProcessId || !g_dynData.ThreadListHead || !g_dynData.ThreadListEntry || !g_dynData.UniqueProcess || !g_dynData.UniqueThread ||
		!g_dynData.correctBuild || !g_dynData.PspCidTable || !g_dynData.ExDestroyHandle)
		return STATUS_NOT_SUPPORTED;

	__try
	{
		void *PspCidTable = *((void **)MAKEPTR(GetKernelBase(), g_dynData.PspCidTable));

		ExDestroyHandle = (fnExExDestroyHandle)((ULONG_PTR)GetKernelBase() + g_dynData.ExDestroyHandle);
		if (!ExDestroyHandle(PspCidTable, pid, NULL))
		{
			return STATUS_ACCESS_DENIED;
		}

		*((ULONG64 *)MAKEPTR(pep, g_dynData.UniqueProcessId)) = 0;  //Avoid CID_HANDLE_DELETION BSOD)

		ThreadListHead = (LIST_ENTRY *)MAKEPTR(pep, g_dynData.ThreadListHead);
		pLE = ThreadListHead;

		while ((pLE = pLE->Flink) != ThreadListHead)
		{
			PETHREAD pet = (PETHREAD)MAKEPTR(pLE, ~((DWORD_PTR)(g_dynData.ThreadListEntry + 0x8)) + 1);
			HANDLE tid = PsGetThreadId(pet);
			HANDLE tpid = PsGetThreadProcessId(pet);

			if (pid == tpid) //Self-Check
			{
				if (!ExDestroyHandle(PspCidTable, tid, NULL))
				{
					return STATUS_ACCESS_DENIED;
				}

				*((ULONG64 *)MAKEPTR(pet, g_dynData.UniqueThread)) = 0;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_ACCESS_DENIED;
	}

	return STATUS_SUCCESS;
}
//获取内存中NT模块的基地址(ntoskrnl.exe)，它与一些偏移量一起用于计算一些未文档化的东西的地址
//Obtain base address of NT Module (ntoskrnl.exe) in memory, this is used with some offsets to calculate the address of some undocumented stuff
PVOID GetKernelBase()
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytes = 0;
	PRTL_PROCESS_MODULES pMods = NULL;
	PVOID checkPtr = NULL;
	UNICODE_STRING routineName;

	//Already found
	if (g_KernelBase != NULL)
		return g_KernelBase;

	RtlUnicodeStringInit(&routineName, L"NtOpenFile");

	checkPtr = MmGetSystemRoutineAddress(&routineName);
	if (checkPtr == NULL)
		return NULL;

	//Protect from UserMode AV
	__try
	{
		status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
		if (bytes == 0)
		{
			return NULL;
		}

		pMods = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, DRIVER_TAG);
		RtlZeroMemory(pMods, bytes);

		status = ZwQuerySystemInformation(SystemModuleInformation, pMods, bytes, &bytes);

		if (NT_SUCCESS(status))
		{
			PRTL_PROCESS_MODULE_INFORMATION pMod = pMods->Modules;

			ULONG i;
			for (i = 0; i < pMods->NumberOfModules; i++)
			{
				if (checkPtr >= pMod[i].ImageBase &&
					checkPtr < (PVOID)((PUCHAR)pMod[i].ImageBase + pMod[i].ImageSize))
				{
					g_KernelBase = pMod[i].ImageBase;
					break;
				}
			}
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	if (pMods)
		ExFreePoolWithTag(pMods, DRIVER_TAG);

	return g_KernelBase;
}

BOOLEAN Is64BitWindows()
{
#if defined(_X86_)
	return FALSE;
#else
	return TRUE;
#endif
}

//Get the revision number from registry 
//"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion"
//Data of value "BuildLabEx" Ex: "9600.17736.amd64fre.winblue_r9.150322-1500"
//9600 is build number, 17736 is revision number
NTSTATUS GetRevisionBuildNO(OUT PULONG pRevisionBuildNo)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING strRegKey = { 0 };
	UNICODE_STRING strRegValue = { 0 };
	UNICODE_STRING strVerVal = { 0 };
	HANDLE hKey = NULL;
	OBJECT_ATTRIBUTES keyAttr = { 0 };
	PKEY_VALUE_FULL_INFORMATION pValueInfo;
	ULONG bytes;
	ULONG i;
	ULONG j;
	PWCHAR pData;

	if (pRevisionBuildNo == 0)
		return STATUS_INVALID_PARAMETER;

	RtlUnicodeStringInit(&strRegKey, L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion");
	RtlUnicodeStringInit(&strRegValue, L"BuildLabEx");

	InitializeObjectAttributes(&keyAttr, &strRegKey, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwOpenKey(&hKey, KEY_READ, &keyAttr);
	if (NT_SUCCESS(status))
	{
		pValueInfo = ExAllocatePoolWithTag(PagedPool, 0x1000, DRIVER_TAG);
		bytes = 0;

		if (pValueInfo)
		{
			status = ZwQueryValueKey(hKey, &strRegValue, KeyValueFullInformation, pValueInfo, 0x1000, &bytes);
			if (NT_SUCCESS(status))
			{
				pData = (PWCHAR)((PUCHAR)pValueInfo->Name + pValueInfo->NameLength);
				for (i = 0; i < pValueInfo->DataLength; i++)
				{
					if (pData[i] == L'.')
					{
						for (j = i + 1; j < pValueInfo->DataLength; j++)
						{
							if (pData[j] == L'.')
							{
								strVerVal.Buffer = &pData[i] + 1;
								strVerVal.Length = strVerVal.MaximumLength = (USHORT)((j - i) * sizeof(WCHAR));
								status = RtlUnicodeStringToInteger(&strVerVal, 10, pRevisionBuildNo);

								goto skip1;
							}
						}
					}
				}

			skip1:;
			}

			ExFreePoolWithTag(pValueInfo, DRIVER_TAG);
		}
		else
			status = STATUS_NO_MEMORY;

		ZwClose(hKey);
	}

	return status;

}

//We init dynamic data structure containing OS version and some fixed offsets used for our hiding methods

//+ PsActiveProcessList using:
//_EPROCESS->ActiveProcessLinks

//+ SessionProcessList using:
//_EPROCESS->SessionProcessLinks

//+ process's handle table removal using:
//ExRemoveHandleTable()
//_EPROCESS->ObjectTable
//+ PspCidTable using:
//_EPROCESS->ThreadListHead
//_ETHREAD->ThreadListEntry
//_ETHREAD->Cid.UniqueThread
//ExDestroyHandle()
//PspCidTable()

//NOTICE: PsActiveProcesSList & SessionProcessList methods can be used dependant on build number of OS
//NOTICE: Process's handle table removal and PspCidTable can be used dependant on build number and revision of OS
//////////////////////////////////////////////////////////////////////////
NTSTATUS InitDynamicData(IN OUT PDYNAMIC_DATA pData)
{
	NTSTATUS status = STATUS_SUCCESS;
	RTL_OSVERSIONINFOEXW verInfo = { 0 };
	ULONG revisionBuildNo = 0;
	BOOLEAN bX64 = FALSE;
	ULONG ver_short;
	CHAR output[256] = { 0 };

	if (pData == NULL)
		return STATUS_INVALID_ADDRESS;

	RtlZeroMemory(pData, sizeof(DYNAMIC_DATA));

	bX64 = Is64BitWindows();

	//Get the build number of OS
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	status = RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo);

	if (status == STATUS_SUCCESS)
	{
		ver_short = (verInfo.dwMajorVersion << 8) | (verInfo.dwMinorVersion << 4) | verInfo.wServicePackMajor;
		pData->ver = (WinVer)ver_short;

		//Get the revision number of OS
		status = GetRevisionBuildNO(&revisionBuildNo);

		//Check current revision number with our known number
		pData->correctBuild = TRUE;

		if (ver_short == WINVER_7)
		{
			if (revisionBuildNo != 16385)
				pData->correctBuild = FALSE;
		}
		else if (ver_short == WINVER_7_SP1)
		{
			if (revisionBuildNo != 17514)
				pData->correctBuild = FALSE;
		}
		else if (ver_short == WINVER_8)
		{
			if (revisionBuildNo != 16384 && revisionBuildNo != 17438)
				pData->correctBuild = FALSE;
		}
		else if (ver_short == WINVER_81)
		{
			if (revisionBuildNo != 16404)
				pData->correctBuild = FALSE;
		}
		else if (ver_short == WINVER_10)
		{
			if (revisionBuildNo != 16431 && revisionBuildNo != 16412)
				pData->correctBuild = FALSE;
		}
		else
			return STATUS_NOT_SUPPORTED;

		switch (ver_short)
		{
			//Windows 7
			//Windows 7 SP1
		case WINVER_7:
		case WINVER_7_SP1:
			pData->ActiveProcessLinks = (bX64) ? 0x188 : 0xB8;
			pData->SessionProcessLinks = (bX64) ? 0x1E0 : 0xE4;
			pData->ObjTable = (bX64) ? 0x200 : 0xF4;
			pData->UniqueProcessId = (bX64) ? 0x180 : 0xB4;
			pData->ThreadListHead = (bX64) ? 0x308 : 0x188;
			pData->ThreadListEntry = (bX64) ? 0x420 : 0x268;
			pData->UniqueProcess = (bX64) ? 0x3B0 + 0x0 : 0x22C + 0x0;
			pData->UniqueThread = (bX64) ? 0x3B0 + 0x8 : 0x22C + 0x4;

			if (revisionBuildNo == 16385 || revisionBuildNo == 17514)
			{
				if (bX64)
				{
					pData->ExDestroyHandle = (ver_short == WINVER_7_SP1) ? 0 : 0x384DB0;
					pData->ExRemoveHandleTable = (ver_short == WINVER_7_SP1) ? 0x32A870 : 0x32D404;
					pData->PspCidTable = (ver_short == WINVER_7_SP1) ? 0 : 0x21FB68;
				}
				else
				{
					pData->ExDestroyHandle = (ver_short == WINVER_7_SP1) ? 0x21365F : 0;
					pData->ExRemoveHandleTable = (ver_short == WINVER_7_SP1) ? 0x266C1F : 0;
					pData->PspCidTable = (ver_short == WINVER_7_SP1) ? 0x1396F4 : 0;
				}
			}
			break;

			//Windows 8.0
		case WINVER_8:
			pData->ActiveProcessLinks = (bX64) ? 0x2E8 : 0xB8;
			pData->SessionProcessLinks = (bX64) ? 0x330 : 0xE0;
			pData->ObjTable = (bX64) ? 0x408 : 0x150;
			pData->UniqueProcessId = (bX64) ? 0x2E0 : 0xB4;
			pData->ThreadListHead = (bX64) ? 0x470 : 0x194;
			pData->ThreadListEntry = (bX64) ? 0x400 : 0x24C;
			pData->UniqueProcess = (bX64) ? 0x398 + 0x0 : 0x214 + 0x0;
			pData->UniqueThread = (bX64) ? 0x398 + 0x8 : 0x214 + 0x4;

			if (revisionBuildNo == 16384)
			{
				pData->ExDestroyHandle = (bX64) ? 0x47969C : 0x24B38A;
				pData->ExRemoveHandleTable = (bX64) ? 0x424440 : 0x2C5065;
				pData->PspCidTable = (bX64) ? 0x356188 : 0x215074;
			}
			if (revisionBuildNo == 17438)
			{
				pData->ExDestroyHandle = (bX64) ? 0x494D20 : 0;
				pData->ExRemoveHandleTable = (bX64) ? 0x4863F0 : 0;
				pData->PspCidTable = (bX64) ? 0x358188 : 0;
			}
			break;

			//Windows 8.1
		case WINVER_81:
			pData->ActiveProcessLinks = (bX64) ? 0x2E8 : 0;
			pData->SessionProcessLinks = (bX64) ? 0x330 : 0;
			pData->ObjTable = (bX64) ? 0x408 : 0;
			pData->UniqueProcessId = (bX64) ? 0x2E0 : 0;
			pData->ThreadListHead = (bX64) ? 0x470 : 0;
			pData->ThreadListEntry = (bX64) ? 0x688 : 0;
			pData->UniqueProcess = (bX64) ? 0x620 + 0x0 : 0;
			pData->UniqueThread = (bX64) ? 0x620 + 0x8 : 0;

			if (revisionBuildNo == 16404)
			{
				pData->ExDestroyHandle = (bX64) ? 0x3D06E0 : 0;
				pData->ExRemoveHandleTable = (bX64) ? 0x40F180 : 0;
				pData->PspCidTable = (bX64) ? 0x34D200 : 0;
			}
			break;

			//Windows 10
		case WINVER_10:
			pData->ActiveProcessLinks = (bX64) ? 0x2F0 : 0;
			pData->SessionProcessLinks = (bX64) ? 0x340 : 0;
			pData->ObjTable = (bX64) ? 0x418 : 0;
			pData->UniqueProcessId = (bX64) ? 0x2E8 : 0;
			pData->ThreadListHead = (bX64) ? 0x480 : 0;
			pData->ThreadListEntry = (bX64) ? 0x690 : 0;
			pData->UniqueProcess = (bX64) ? 0x628 + 0x0 : 0;
			pData->UniqueThread = (bX64) ? 0x628 + 0x8 : 0;

			if (revisionBuildNo == 16431 || revisionBuildNo == 16412)
			{
				pData->ExDestroyHandle = (bX64) ? 0x4BD5E8 : 0;
				pData->ExRemoveHandleTable = (bX64) ? 0x4C36AC : 0;
				pData->PspCidTable = (bX64) ? 0x3C5318 : 0;
			}
			break;

		default:
			break;
		}
	}

	return status;
}