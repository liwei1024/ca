/***************************************************************************************************
Module: DriverEntry.c
Author: lw @ 2018
***************************************************************************************************/
#include "kca.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

KCA_TARGET_PROCESS_INFO_STRUCT g_TargetProcessInfo;
UNICODE_STRING SymbolicLinkName;
HANDLE g_SelfProcessId = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////

VOID CreateProcessNotifyEx(
	_Inout_  PEPROCESS              Process,
	_In_     HANDLE                 ProcessId,
	_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
) {
	if (NULL != CreateInfo)
	{
		if (wcsstr(CreateInfo->ImageFileName->Buffer, TARGET_PROCESS_NAME))
		{
			g_TargetProcessInfo.ProcessStatus = TRUE;
			g_TargetProcessInfo.ProcessId = ProcessId;
			g_TargetProcessInfo.Process = Process;
#ifdef DEBUG
			dprintf("进程已经加载");
#endif // DEBUG
			
		}
	}
	else {
		if (ProcessId == g_SelfProcessId) {
			ProtectProcess(FALSE, NULL);
		}
		
		if (ProcessId == g_TargetProcessInfo.ProcessId)
		{
			g_TargetProcessInfo.ProcessHandle = NULL;
			g_TargetProcessInfo.MainThreadHandle = NULL;
			g_TargetProcessInfo.ProcessStatus = FALSE;
#ifdef DEBUG
			dprintf("进程已经退出");
#endif // DEBUG
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VOID NotifyImageLoadCallback(
	_In_opt_ PUNICODE_STRING FullImageName,
	_In_ HANDLE ProcessId,                // pid into which image is being mapped
	_In_ PIMAGE_INFO ImageInfo
)
{
	if (wcsstr(FullImageName->Buffer, TARGET_PROCESS_FILE_NAME) && wcsstr(FullImageName->Buffer, L"Device"))
	{
		g_TargetProcessInfo.MainThreadId = PsGetCurrentThreadId();
		g_TargetProcessInfo.MainThread = PsGetCurrentThread();
		g_TargetProcessInfo.ProcessBaseAddress = ImageInfo->ImageBase;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS DefaultDispatchFunction(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VOID DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject
)
{
	if (!NT_SUCCESS(PsRemoveLoadImageNotifyRoutine(NotifyImageLoadCallback)))
	{
#ifdef DEBUG
		dprintf("Delete NotifyImageLoadCallback Failure");
#endif // DEBUG
	}
	// 删除创建进程回调
	if (!NT_SUCCESS(PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, TRUE))) {
#ifdef DEBUG
		dprintf("Delete CreateProcessNotifyRoutineEx Failure");
#endif // DEBUG
	}
	// 删除符号链接
	if (!NT_SUCCESS(IoDeleteSymbolicLink(&SymbolicLinkName)))
	{
#ifdef DEBUG
		dprintf("Delete SymbolicLinkName Failure");
#endif // DEBUG
	}
	// 删除设备
	IoDeleteDevice(DriverObject->DeviceObject);

	ProtectProcess(FALSE,NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//PDRIVER_OBJECT g_pDriverObject = NULL;
//#define DELAY_ONE_MICROSECOND 	(-10)
//#define DELAY_ONE_MILLISECOND	(DELAY_ONE_MICROSECOND*1000)
//VOID KernelSleep(LONG msec)
//{
//	LARGE_INTEGER my_interval;
//	my_interval.QuadPart = DELAY_ONE_MILLISECOND;
//	my_interval.QuadPart *= msec;
//	KeDelayExecutionThread(KernelMode, 0, &my_interval);
//}
//
//VOID
//DelObject(
//	_In_ PVOID StartContext
//)
//{
//	PULONG_PTR pZero = NULL;
//	KernelSleep(5000);
//	ObMakeTemporaryObject(g_pDriverObject);
//	dprintf("test seh.\n");
//	__try {
//		*pZero = 0x100;
//	}
//	__except (1)
//	{
//		dprintf("seh success.\n");
//	}
//}
//
//VOID Reinitialize(
//	_In_     PDRIVER_OBJECT        pDriverObject,
//	_In_opt_ PVOID                 Context,
//	_In_     ULONG                 Count
//)
//{
//	HANDLE hThread = NULL;
//	PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, DelObject, NULL);
//	if ((NtBuildNumber & 0x0000FFFF) < 8000)
//		HideDriverWin7(pDriverObject);
//	else
//		HideDriverWin10(pDriverObject);
//}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceName;
	//PLDR_DATA_TABLE_ENTRY64 ldr;

	DriverObject->DriverUnload = DriverUnload;

	//dprintf("NtBuildNumber->:%u", NtBuildNumber & 0x0000FFFF);
	
	//=====绕过mmverifycallbackfunction=======//
	//*(PULONG)((PCHAR)DriverObject->DriverSection + 13 * sizeof(void*)) |= 0x20;//不改这个用不了这个函数
	/*ldr = (PLDR_DATA_TABLE_ENTRY64)DriverObject->DriverSection; 
	ldr->Flags |= 0x20;*/
	//=====绕过mmverifycallbackfunction=======//
	// 创建进程回调
	Status = PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, FALSE);
	if (!NT_SUCCESS(Status))
	{
		#ifdef DEBUG
				dprintf("Failed to call PsSetCreateProcessNotifyRoutineEx, error code = 0x%08X", Status);
		#endif // DEBUG
	}

	// 镜像加载通知回调函数
	Status = PsSetLoadImageNotifyRoutine(NotifyImageLoadCallback);
	if (!NT_SUCCESS(Status))
	{
		#ifdef DEBUG
				dprintf("Create LoadImageNotifyRoutine Failure");
		#endif // DEBUG
	}
	// 创建虚拟设备
	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
	Status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(Status))
	{
		#ifdef DEBUG
				dprintf("Create DeviceObject Failure");
		#endif // DEBUG
		return Status;
	}
	// 创建设备的符号链接
	RtlInitUnicodeString(&SymbolicLinkName, SYMBOLIC_LINK_NAME);
	Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
	if (!NT_SUCCESS(Status))
	{
		#ifdef DEBUG
				dprintf("Create SymbolicLink Failure");
		#endif // DEBUG
		IoDeleteDevice(DeviceObject);
		return Status;
	}
	// 处理Irp请求
	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = DefaultDispatchFunction;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KcaDispatchDeviceControl;
	

	DriverObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
	#ifdef DEBUG
		dprintf("Loader Success!");
	#endif // DEBUG
	/*g_pDriverObject = DriverObject;
	IoRegisterDriverReinitialization(DriverObject, Reinitialize, NULL);*/
	return Status;
}

//////////////////////////////////////////// End Of File ///////////////////////////////////////////