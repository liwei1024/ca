#ifndef KCA_API_H
#define KCA_API_H


//#define TARGET_PROCESS_NAME L"YoudaoDict.exe"
//#define TARGET_PROCESS_FILE_NAME L"YoudaoDict.exe"
//#define TARGET_PROCESS_NAME L"cheatengine-x86_64.exe"
#define DRIVER_FILE_NAME L"ca.sys"
#define DEVICE_SHOST_NAME L"ca"
#define DEVICE_NAME (L"\\Device\\" DEVICE_SHOST_NAME)
#define SYMBOLIC_LINK_SHORT_NAME L"ca"
#define SYMBOLIC_LINK_NAME (L"\\DosDevices\\" SYMBOLIC_LINK_SHORT_NAME)


#define KCTL_CODE(x) CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800 + x, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define KCA_READ_VIRTUAL_MEMORY KCTL_CODE(1)
#define KCA_WRITE_VIRTUAL_MEMORY KCTL_CODE(2)
#define KCA_GET_PROCESS_ID KCTL_CODE(3)
#define KCA_GET_PROCESS_HANDLE KCTL_CODE(4)
#define KCA_GET_THREAD_HANDLE KCTL_CODE(5)
#define KCA_PROTECT_CURRENT_PROCESS KCTL_CODE(6)
#define KCA_PROTECT_CURRENT_PROCESS_FILE KCTL_CODE(7)
#define KCA_UN_PROTECT_CURRENT_PROCESS_FILE KCTL_CODE(8)
#define KCA_MODIFY_CURRENT_PROCESS_PATH KCTL_CODE(9)
//#define KCA_SET_PROTECTION KCTL_CODE(10)

typedef struct _KCA_REMOTE_CALL_STRUCT {
	IN HANDLE Process;
	IN HANDLE Thread;
	IN PVOID CallSite; //call 地址
	IN ULONG ArgumentCount;// 参数数量
	IN PULONG Arguments;//参数
	IN BOOLEAN PassContext;
	IN BOOLEAN AlreadySuspended;
}KCA_REMOTE_CALL_STRUCT,*PKCA_REMOTE_CALL_STRUCT;

//typedef struct _READ_VIRTUAL_MEMORY_STRUCT
//{
//	ULONG64 Address;
//	PVOID64 Response;
//	ULONG64 Size;
//
//}KCA_READ_VIRTUAL_MEMORY_STRUCT, *PKCA_READ_VIRTUAL_MEMORY_STRUCT;
//
//typedef struct _WRITE_VIRTUAL_MEMORY_STRUCT
//{
//	ULONG64 Address;
//	PVOID64 Value;
//	ULONG64 Size;
//
//}KCA_WRITE_VIRTUAL_MEMORY_STRUCT, *PKCA_WRITE_VIRTUAL_MEMORY_STRUCT;

typedef struct _READ_VIRTUAL_MEMORY_STRUCT
{
	ULONG Address;
	PVOID Response;
	SIZE_T Size;

}KCA_READ_VIRTUAL_MEMORY_STRUCT, *PKCA_READ_VIRTUAL_MEMORY_STRUCT;

typedef struct _WRITE_VIRTUAL_MEMORY_STRUCT
{
	ULONG Address;
	PVOID Value;
	SIZE_T Size;

}KCA_WRITE_VIRTUAL_MEMORY_STRUCT, *PKCA_WRITE_VIRTUAL_MEMORY_STRUCT;

///// <summary>
///// Policy activation option
///// </summary>
//typedef enum _PolicyOpt
//{
//	Policy_Disable,
//	Policy_Enable,
//	Policy_Keep,        // Don't change current value
//} PolicyOpt;
//
///// <summary>
///// Input for IOCTL_BLACKBONE_SET_PROTECTION
///// </summary>
//typedef struct _SET_PROC_PROTECTION
//{
//	ULONG pid;              // Process ID
//	PolicyOpt protection;   // Process protection
//	PolicyOpt dynamicCode;  // DynamiCode policy
//	PolicyOpt signature;    // BinarySignature policy
//} SET_PROC_PROTECTION, *PSET_PROC_PROTECTION;

#endif // !KCA_API_H





