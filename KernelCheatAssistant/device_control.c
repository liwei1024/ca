#include "kca.h"
#include "set_protection.h"


NTSTATUS KcaDispatchDeviceControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PIO_STACK_LOCATION Stack;
	PKCA_READ_VIRTUAL_MEMORY_STRUCT rvms;
	PKCA_WRITE_VIRTUAL_MEMORY_STRUCT wvms;
	PKCA_REMOTE_CALL_STRUCT rcs;
	ULONG IoControlCode;

	//得到当前堆栈
	Stack = IoGetCurrentIrpStackLocation(Irp);

	if (Stack && Irp->AssociatedIrp.SystemBuffer)
	{
		g_SelfProcessId = PsGetCurrentProcessId();
		IoControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;
		if (
			g_TargetProcessInfo.ProcessStatus == TRUE ||
			IoControlCode == KCA_PROTECT_CURRENT_PROCESS ||
			IoControlCode == KCA_MODIFY_CURRENT_PROCESS_PATH 
			//|| IoControlCode == KCA_SET_PROTECTION
			)
		{
			switch (IoControlCode)
			{
			case KCA_READ_VIRTUAL_MEMORY:
			{
				rvms = (PKCA_READ_VIRTUAL_MEMORY_STRUCT)Irp->AssociatedIrp.SystemBuffer;
				if (rvms)
				{
					Status = KcaReadVirtualMemory(rvms);
				}
				Irp->IoStatus.Information = sizeof(KCA_READ_VIRTUAL_MEMORY_STRUCT);
			}
			break;
			case KCA_WRITE_VIRTUAL_MEMORY:
			{
				wvms = (PKCA_WRITE_VIRTUAL_MEMORY_STRUCT)Irp->AssociatedIrp.SystemBuffer;
				if (wvms)
				{
					Status = KcaWriteVirtualMemory(wvms);
				}
				Irp->IoStatus.Information = sizeof(KCA_WRITE_VIRTUAL_MEMORY_STRUCT);
			}
			break;
			case KCA_GET_PROCESS_ID:
			{
				if (g_TargetProcessInfo.ProcessStatus == TRUE)
				{
					Status = STATUS_SUCCESS;
					*(ULONG*)Irp->AssociatedIrp.SystemBuffer = (ULONG)(ULONG_PTR)g_TargetProcessInfo.ProcessId;
				}
				Irp->IoStatus.Information = sizeof(ULONG);
			}
			break;
			case KCA_GET_PROCESS_HANDLE:
			{
				if (g_TargetProcessInfo.ProcessStatus == TRUE) {
					Status = STATUS_SUCCESS;
					*(HANDLE*)Irp->AssociatedIrp.SystemBuffer = KcaGetProcessHandle(g_TargetProcessInfo.Process);
				}
				Irp->IoStatus.Information = sizeof(HANDLE);
			}
			break;
			case KCA_GET_THREAD_HANDLE:
			{
				if (g_TargetProcessInfo.ProcessStatus == TRUE) {
					Status = STATUS_SUCCESS;
					*(HANDLE*)Irp->AssociatedIrp.SystemBuffer = KcaGetThreadHandle(g_TargetProcessInfo.MainThread);
				}
				Irp->IoStatus.Information = sizeof(HANDLE);
			}
			break;
			case KCA_PROTECT_CURRENT_PROCESS:
			{
				BOOLEAN Enable = TRUE;
				if (Irp->AssociatedIrp.SystemBuffer)
				{
					memcpy(&Enable, Irp->AssociatedIrp.SystemBuffer, sizeof(Enable));
				}
				ProtectProcess(Enable, PsGetCurrentProcessId());
				Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = 0;
			}
			break;
			case KCA_MODIFY_CURRENT_PROCESS_PATH:
			{
				ULONG ProcessId = 0;
				if (Irp->AssociatedIrp.SystemBuffer)
				{
					memcpy(&ProcessId, Irp->AssociatedIrp.SystemBuffer, sizeof(ProcessId));
				}
				//dprintf("pid->:%d", ProcessId);
				PathModification((HANDLE)ProcessId);
				Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = 0;
			}
			break; 
			/*case KCA_SET_PROTECTION:
			{
				Status = BBSetProtection((PSET_PROC_PROTECTION)Irp->AssociatedIrp.SystemBuffer);
				Irp->IoStatus.Information = 0;
			}
			break;*/
			//case KCA_PROTECT_CURRENT_PROCESS_FILE:
			//{
			//	//dprintf("文件保护");
			//	KcaProtectFileByObRegisterCallbacks(PsGetCurrentProcessId());
			//	Status = STATUS_SUCCESS;
			//	Irp->IoStatus.Information = 0;
			//}
			//break;
			//case KCA_UN_PROTECT_CURRENT_PROCESS_FILE:
			//{
			//	//dprintf("解除文件保护");
			//	KcaUnProtectFileByObRegisterCallbacks();
			//	Status = STATUS_SUCCESS;
			//	Irp->IoStatus.Information = 0;
			//}
			//break;
			default:
				Status = STATUS_INVALID_DEVICE_REQUEST;
				break;
			}
		}
	}
	else {
		Status = STATUS_NOT_SUPPORTED;
	}
	Irp->IoStatus.Status = Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}