#include "kca.h"

HANDLE KcaGetThreadHandle(PETHREAD Thread)
{
	HANDLE ThreadHandle = NULL;
	ObOpenObjectByPointer(
		Thread,
		0,
		NULL,
		THREAD_ALL_ACCESS,
		*PsThreadType,
		KernelMode,
		&ThreadHandle
	);
	return ThreadHandle;
}