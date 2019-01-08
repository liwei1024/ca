#include "kca.h"

#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  
#define PROCESS_SET_LIMITED_INFORMATION    (0x2000) 


PVOID g_RegistrationHandle = NULL;
HANDLE g_ProtegeProcessId = NULL;

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) 
{
	if (g_ProtegeProcessId == NULL)
	{
		return OB_PREOP_SUCCESS;
	}
	HANDLE pid = PsGetProcessId((PEPROCESS)OperationInformation->Object);
	UNREFERENCED_PARAMETER(RegistrationContext);
	if (OperationInformation->ObjectType == *PsProcessType && pid == g_ProtegeProcessId)
	{
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			ACCESS_MASK DesiredAccess = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;

			DesiredAccess = DesiredAccess & (PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE);
			if (DesiredAccess) {
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~DesiredAccess;// DesiredAccess;
			}
		}
		else if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
		{
			ACCESS_MASK DesiredAccess = OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess;

			DesiredAccess = DesiredAccess & (PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE);
			if (DesiredAccess) {
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~DesiredAccess;// DesiredAccess;
			}
			
		}
	}
	return OB_PREOP_SUCCESS;
}

VOID ProcessPostCallback(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation)
{
	
	//dprintf("ProcessPostCallback");
}

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) 
{
	ACCESS_MASK DesiredAccess;
	if (g_ProtegeProcessId == NULL)
		return OB_PREOP_SUCCESS;
	if (PsGetCurrentProcessId() == g_ProtegeProcessId)
		return OB_PREOP_SUCCESS;
	if (OperationInformation->ObjectType == *PsThreadType)
	{
		if (g_ProtegeProcessId == PsGetThreadProcessId(OperationInformation->Object))
		{
			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
			{
				DesiredAccess = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;

				DesiredAccess = DesiredAccess & (THREAD_SET_LIMITED_INFORMATION | THREAD_QUERY_LIMITED_INFORMATION);
				if (DesiredAccess) {
					OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~DesiredAccess;
				}
			}
			else if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
			{
				DesiredAccess = OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess;

				DesiredAccess = DesiredAccess & (THREAD_SET_LIMITED_INFORMATION | THREAD_QUERY_LIMITED_INFORMATION);
				if (DesiredAccess) {
					OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess &= ~DesiredAccess;
				}
			}
		}
	}
	return OB_PREOP_SUCCESS;
}

VOID ThreadPostCallback(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation)
{
	//dprintf("ThreadPostCallback");
}

OB_PREOP_CALLBACK_STATUS FilePreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	PFILE_OBJECT FileObject = OperationInformation->Object;
	ACCESS_MASK amask = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
	UNREFERENCED_PARAMETER(RegistrationContext);

	if (OperationInformation->ObjectType != *IoFileObjectType)
	{
		return OB_PREOP_SUCCESS;
	}
	//过滤无效指针
	if (FileObject->FileName.Buffer == NULL ||
		!MmIsAddressValid(FileObject->FileName.Buffer) ||
		FileObject->DeviceObject == NULL ||
		!MmIsAddressValid(FileObject->DeviceObject))
	{
		return OB_PREOP_SUCCESS;
	}

	//dprintf("[PRE] object:%p access:%X file:%ws\n", FileObject, amask, &FileObject->FileName);
	return OB_PREOP_SUCCESS;
}

VOID FilePostCallback(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation)
{
	//dprintf("FilePostCallback");
}

//VOID EnableObType2(POBJECT_TYPE ObjectType)
//{
//	POBJECT_TYPE_TEMP  ObjectTypeTemp = (POBJECT_TYPE_TEMP)ObjectType;
//	ObjectTypeTemp->TypeInfo.SupportsObjectCallbacks = 1; 
//}

NTSTATUS ProtectProcess(BOOLEAN Enable,HANDLE ProcessId)
{
	NTSTATUS Status = STATUS_SUCCESS;
	if (Enable == TRUE && ProcessId != NULL)
	{
		OB_CALLBACK_REGISTRATION obReg;
		OB_OPERATION_REGISTRATION opReg[2];
		//POBJECT_TYPE FileObject;
		//PFILE_OBJECT FileObject;
		
		memset(&opReg, 0, sizeof(opReg)); //初始化结构体变量

		opReg[0].ObjectType = PsProcessType;
		opReg[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		opReg[0].PreOperation = ProcessPreCallback;
		opReg[0].PostOperation = ProcessPostCallback;

		opReg[1].ObjectType = PsThreadType;
		opReg[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		opReg[1].PreOperation = ThreadPreCallback;
		opReg[1].PostOperation = ThreadPostCallback;

		/*EnableObType2(*IoFileObjectType);//win7 可用
		opReg[2].ObjectType = IoFileObjectType;
		opReg[2].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		opReg[2].PreOperation = FilePreCallback;
		opReg[2].PostOperation = FilePostCallback;*/


		memset(&obReg, 0, sizeof(obReg));//初始化结构体变量

		obReg.Version = OB_FLT_REGISTRATION_VERSION;//ObGetFilterVersion();
		obReg.OperationRegistrationCount = (sizeof(opReg) / sizeof(OB_OPERATION_REGISTRATION));
		obReg.RegistrationContext = NULL;
		RtlInitUnicodeString(&obReg.Altitude, L"401900");
		obReg.OperationRegistration = opReg;

		Status = ObRegisterCallbacks(&obReg, &g_RegistrationHandle); //在这里注册回调函数
		if (!NT_SUCCESS(Status))
		{
#ifdef DEBUG
			dprintf("保护开启失败 Status->:%d", Status);
#endif // DEBUG
		}
		else {
			g_ProtegeProcessId = ProcessId;
#ifdef DEBUG
			dprintf("保护开启成功 ProcessId->:%d", g_ProtegeProcessId);
#endif // DEBUG
		}
	}
	else {
		if (g_RegistrationHandle != NULL)
		{
			ObUnRegisterCallbacks(g_RegistrationHandle);
			g_RegistrationHandle = NULL;
		}
	}
	return Status;
}