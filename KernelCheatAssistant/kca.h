#ifndef KCA_H
#define KCA_H
#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "kca_api.h"
#include "hide.h"
//#define DEBUG
#ifdef DEBUG
#define dprintf(Format, ...) DbgPrint("KCA: " Format "\n", __VA_ARGS__)
#endif // DEBUG

#define MAX_STRING_LENGTH			512

typedef struct _TARGET_PROCESS_INFO
{
	PEPROCESS Process;
	HANDLE ProcessId;
	HANDLE ProcessHandle;
	PVOID ProcessBaseAddress;

	HANDLE MainThreadId;
	PETHREAD MainThread;
	HANDLE MainThreadHandle;

	BOOLEAN ProcessStatus;

}KCA_TARGET_PROCESS_INFO_STRUCT, *PKCA_TARGET_PROCESS_INFO_STRUCT;

extern KCA_TARGET_PROCESS_INFO_STRUCT g_TargetProcessInfo;

extern NTKERNELAPI ULONG NtBuildNumber;//系统版本号

extern HANDLE g_SelfProcessId;

// system
NTKERNELAPI NTSTATUS  MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	CONST VOID * SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTKERNELAPI PCHAR PsGetProcessImageFileName(
	PEPROCESS Process
);

NTKERNELAPI NTSTATUS PsReferenceProcessFilePointer(
	IN PEPROCESS Process,
	OUT PVOID *pFilePointer
);

NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(
	__in PEPROCESS Process
);

NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(
	IN PEPROCESS Process
);
NTKERNELAPI NTSTATUS NTAPI ZwQueryInformationProcess(
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
);

//NTSTATUS RtlUnicodeStringInit(
//	_Out_    PUNICODE_STRING  DestinationString,
//	_In_opt_ NTSTRSAFE_PCWSTR pszSrc
//);


//device_control
NTSTATUS KcaDispatchDeviceControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);
// virtual_memory
NTSTATUS KcaReadVirtualMemory(
	PKCA_READ_VIRTUAL_MEMORY_STRUCT rvms
);

NTSTATUS KcaWriteVirtualMemory(
	PKCA_WRITE_VIRTUAL_MEMORY_STRUCT wvms
);

// utils
PCHAR GetProcessNameByProcessId(
	HANDLE hProcessId
);

BOOLEAN GetPathByFileObject(
	PFILE_OBJECT FileObject,
	WCHAR* wzPath
);
BOOLEAN GetProcessPathBySectionObject(
	HANDLE ulProcessID,
	WCHAR* wzProcessPath
);
// process
HANDLE KcaGetProcessHandle(
	PEPROCESS Process
);
//VOID KcaUnProtectProcess();
//NTSTATUS KcaProtectProcess(
//	HANDLE ProtegeProcessId
//);
// thread
HANDLE KcaGetThreadHandle(
	PETHREAD Thread
);
// file
//NTSTATUS KcaProtectFileByObRegisterCallbacks(
//	HANDLE ProcessId
//);
//VOID KcaUnProtectFileByObRegisterCallbacks();
//NTSTATUS
//IrpCreateFile(
//	OUT PFILE_OBJECT *FileObject,
//	IN ACCESS_MASK DesiredAccess,
//	IN PUNICODE_STRING FilePath,
//	OUT PIO_STATUS_BLOCK IoStatusBlock,
//	IN PLARGE_INTEGER AllocationSize OPTIONAL,
//	IN ULONG FileAttributes,
//	IN ULONG ShareAccess,
//	IN ULONG CreateDisposition,
//	IN ULONG CreateOptions,
//	IN PVOID EaBuffer OPTIONAL,
//	IN ULONG EaLength
//);
// call_back
NTSTATUS ProtectProcess(
	BOOLEAN Enable, 
	HANDLE ProcessId
);
// path_mod 
BOOLEAN PathModification(HANDLE pid);//伪装程序路径

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE,MmCopyVirtualMemory)
#pragma alloc_text(PAGE,KcaDispatchDeviceControl)
#pragma alloc_text(PAGE,KcaReadVirtualMemory)
#pragma alloc_text(PAGE,KcaWriteVirtualMemory)
#endif

#endif // !KCA_H
