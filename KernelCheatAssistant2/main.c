#include "kca.h"


/// <summary>
/// 驱动入口 注册驱动
/// </summary>
/// <param name="DriverObject">驱动对象</param>
/// <param name="RegistryPath">注册路径y</param>
/// <returns>NTSTATUS</returns>
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject, 
	IN PUNICODE_STRING RegistryPath
)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS Status = STATUS_SUCCESS;



	return Status;
}