#ifndef __ROUTINES_H__
#define __ROUTINES_H__

#include "kca.h"

#define MAKEPTR( ptr, addValue ) ( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section; //Not filled in
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef enum _WinVer
{
	WINVER_7 = 0x0610,
	WINVER_7_SP1 = 0x0611,
	WINVER_8 = 0x0620,
	WINVER_81 = 0x0630,
	WINVER_10 = 0x0A00,
} WinVer;

typedef struct _DYNAMIC_DATA
{
	WinVer  ver;            // OS version
	BOOLEAN correctBuild;   // OS kernel build number is correct and supported

	ULONG ActiveProcessLinks;	// EPROCESS::ActiveProcessLinks
	ULONG SessionProcessLinks;	// EPROCESS::SessionProcessLinks
	ULONG ObjTable;				// EPROCESS::ObjectTable
	ULONG UniqueProcessId;		// EPROCESS::UniqueProcessId
	ULONG ThreadListHead;		// EPROCESS::ThreadListHead
	ULONG ThreadListEntry;		// ETHREAD::ThreadListEntry
	ULONG UniqueProcess;		// ETHREAD::Cid::UniqueProcess
	ULONG UniqueThread;			// ETHREAD::Cid::UniqueThread
	ULONG ExDestroyHandle;		// ExDestroyHandle offset
	ULONG ExRemoveHandleTable;	// ExRemoveHandleTable offset
	ULONG PspCidTable;			// PspCidTable offset
} DYNAMIC_DATA, *PDYNAMIC_DATA;


typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemModuleInformation = 0xb,
} SYSTEM_INFORMATION_CLASS;


extern DYNAMIC_DATA	 g_dynData;

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);
PVOID GetKernelBase();
typedef struct _HANDLE_TABLE
{
	ULONG NextHandleNeedingPool;
	long ExtraInfoPages;
	LONG_PTR TableCode;
	//???
} HANDLE_TABLE, *PHANDLE_TABLE;

typedef NTSTATUS(NTAPI* fnExRemoveHandleTable)(IN PHANDLE_TABLE pTable);
typedef NTSTATUS(NTAPI* fnExExDestroyHandle)(IN PHANDLE_TABLE pTable,
	IN HANDLE Handle,
	IN PVOID HandleTableEntry);

NTSTATUS PsActiveProcessListHiding(PEPROCESS pep, BOOLEAN bHiding);
NTSTATUS SessionProcessListHiding(PEPROCESS pep, BOOLEAN bHiding);
NTSTATUS RemoveProcessHandleTable(PEPROCESS pep);
NTSTATUS RemovePspCidTable(PEPROCESS pep, HANDLE pid);
BOOLEAN Is64BitWindows();
NTSTATUS GetRevisionBuildNO(OUT PULONG pRevisionBuildNo);
NTSTATUS InitDynamicData(IN OUT PDYNAMIC_DATA pData);

#endif /// __ROUTINES_H__