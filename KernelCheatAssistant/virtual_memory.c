#include "kca.h"

#define KCA_STACK_COPY_BYTES 0x200
#define KCA_POOL_COPY_BYTES 0x10000
#define KCA_MAPPED_COPY_PAGES 14
#define KCA_POOL_COPY_THRESHOLD 0x3ff

ULONG KcaGetCopyExceptionInfo(
	_In_ PEXCEPTION_POINTERS ExceptionInfo,
	_Out_ PBOOLEAN HaveBadAddress,
	_Out_ PULONG_PTR BadAddress
)
{
	PEXCEPTION_RECORD exceptionRecord;

	*HaveBadAddress = FALSE;
	exceptionRecord = ExceptionInfo->ExceptionRecord;

	if ((exceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION) ||
		(exceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) ||
		(exceptionRecord->ExceptionCode == STATUS_IN_PAGE_ERROR))
	{
		if (exceptionRecord->NumberParameters > 1)
		{
			/* We have the address. */
			*HaveBadAddress = TRUE;
			*BadAddress = exceptionRecord->ExceptionInformation[1];
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

NTSTATUS KcaCopyVirtualMemory(
	_In_ PEPROCESS FromProcess,
	_In_ PVOID FromAddress,
	_In_ PEPROCESS ToProcess,
	_In_ PVOID ToAddress,
	_In_ SIZE_T BufferLength,
	_In_ KPROCESSOR_MODE AccessMode,
	_Out_ PSIZE_T ReturnLength
)
{
	UCHAR stackBuffer[KCA_STACK_COPY_BYTES];
	PVOID buffer;
	PFN_NUMBER mdlBuffer[(sizeof(MDL) / sizeof(PFN_NUMBER)) + KCA_MAPPED_COPY_PAGES + 1];
	PMDL mdl = (PMDL)mdlBuffer;
	PVOID mappedAddress;
	SIZE_T mappedTotalSize;
	SIZE_T blockSize;
	SIZE_T stillToCopy;
	KAPC_STATE apcState;
	PVOID sourceAddress;
	PVOID targetAddress;
	BOOLEAN doMappedCopy;
	BOOLEAN pagesLocked;
	BOOLEAN copyingToTarget = FALSE;
	BOOLEAN probing = FALSE;
	BOOLEAN mapping = FALSE;
	BOOLEAN haveBadAddress;
	ULONG_PTR badAddress;

	PAGED_CODE();

	sourceAddress = FromAddress;
	targetAddress = ToAddress;

	// We don't check if buffer == NULL when freeing. If buffer doesn't need to be freed, set to
	// stackBuffer, not NULL.
	buffer = stackBuffer;

	mappedTotalSize = (KCA_MAPPED_COPY_PAGES - 2) * PAGE_SIZE;

	if (mappedTotalSize > BufferLength)
		mappedTotalSize = BufferLength;

	stillToCopy = BufferLength;
	blockSize = mappedTotalSize;

	while (stillToCopy)
	{
		// If we're at the last copy block, copy the remaining bytes instead of the whole block
		// size.
		if (blockSize > stillToCopy)
			blockSize = stillToCopy;

		// Choose the best method based on the number of bytes left to copy.
		if (blockSize > KCA_POOL_COPY_THRESHOLD)
		{
			doMappedCopy = TRUE;
		}
		else
		{
			doMappedCopy = FALSE;

			if (blockSize <= KCA_STACK_COPY_BYTES)
			{
				if (buffer != stackBuffer)
					ExFreePoolWithTag(buffer, 'KCA');

				buffer = stackBuffer;
			}
			else
			{
				// Don't allocate the buffer if we've done so already. Note that the block size
				// never increases, so this allocation will always be OK.
				if (buffer == stackBuffer)
				{
					// Keep trying to allocate a buffer.

					while (TRUE)
					{
						buffer = ExAllocatePoolWithTag(NonPagedPool, blockSize, 'KCA');

						// Stop trying if we got a buffer.
						if (buffer)
							break;

						blockSize /= 2;

						// Use the stack buffer if we can.
						if (blockSize <= KCA_STACK_COPY_BYTES)
						{
							buffer = stackBuffer;
							break;
						}
					}
				}
			}
		}

		// Reset state.
		mappedAddress = NULL;
		pagesLocked = FALSE;
		copyingToTarget = FALSE;

		KeStackAttachProcess(FromProcess, &apcState);

		__try
		{
			// Probe only if this is the first time.
			if (sourceAddress == FromAddress && AccessMode != KernelMode)
			{
				probing = TRUE;
				ProbeForRead(sourceAddress, BufferLength, sizeof(UCHAR));
				probing = FALSE;
			}

			if (doMappedCopy)
			{
				// Initialize the MDL.
				MmInitializeMdl(mdl, sourceAddress, blockSize);
				MmProbeAndLockPages(mdl, AccessMode, IoReadAccess);
				pagesLocked = TRUE;

				// Map the pages.
				mappedAddress = MmMapLockedPagesSpecifyCache(
					mdl,
					KernelMode,
					MmCached,
					NULL,
					FALSE,
					HighPagePriority
				);

				if (!mappedAddress)
				{
					// Insufficient resources; exit.
					mapping = TRUE;
					ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
				}
			}
			else
			{
				memcpy(buffer, sourceAddress, blockSize);
			}

			KeUnstackDetachProcess(&apcState);

			// Attach to the target process and copy the contents out.
			KeStackAttachProcess(ToProcess, &apcState);

			// Probe only if this is the first time.
			if (targetAddress == ToAddress && AccessMode != KernelMode)
			{
				probing = TRUE;
				ProbeForWrite(targetAddress, BufferLength, sizeof(UCHAR));
				probing = FALSE;
			}

			// Copy the data.
			copyingToTarget = TRUE;

			if (doMappedCopy)
				memcpy(targetAddress, mappedAddress, blockSize);
			else
				memcpy(targetAddress, buffer, blockSize);
		}
		__except (KcaGetCopyExceptionInfo(
			GetExceptionInformation(),
			&haveBadAddress,
			&badAddress
		))
		{
			KeUnstackDetachProcess(&apcState);

			// If we mapped the pages, unmap them.
			if (mappedAddress)
				MmUnmapLockedPages(mappedAddress, mdl);

			// If we locked the pages, unlock them.
			if (pagesLocked)
				MmUnlockPages(mdl);

			// If we allocated pool storage, free it.
			if (buffer != stackBuffer)
				ExFreePoolWithTag(buffer, 'KCA');

			// If we failed when probing or mapping, return the error status.
			if (probing || mapping)
				return GetExceptionCode();

			// Determine which copy failed.
			if (copyingToTarget && haveBadAddress)
			{
				*ReturnLength = (ULONG)(badAddress - (ULONG_PTR)sourceAddress);
			}
			else
			{
				*ReturnLength = BufferLength - stillToCopy;
			}

			return STATUS_PARTIAL_COPY;
		}

		KeUnstackDetachProcess(&apcState);

		if (doMappedCopy)
		{
			MmUnmapLockedPages(mappedAddress, mdl);
			MmUnlockPages(mdl);
		}

		stillToCopy -= blockSize;
		sourceAddress = (PVOID)((ULONG_PTR)sourceAddress + blockSize);
		targetAddress = (PVOID)((ULONG_PTR)targetAddress + blockSize);
	}

	if (buffer != stackBuffer)
		ExFreePoolWithTag(buffer, 'KCA');

	*ReturnLength = BufferLength;

	return STATUS_SUCCESS;
}

NTSTATUS KcaReadVirtualMemory(
	PKCA_READ_VIRTUAL_MEMORY_STRUCT rvms
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	SIZE_T Bytes;
	if (g_TargetProcessInfo.ProcessStatus == TRUE)
	{
		Status = MmCopyVirtualMemory(g_TargetProcessInfo.Process, (CONST PVOID)rvms->Address, PsGetCurrentProcess(), (SIZE_T)rvms->Response, rvms->Size, KernelMode, &Bytes);
	}
	return Status;

}

NTSTATUS KcaWriteVirtualMemory(
	PKCA_WRITE_VIRTUAL_MEMORY_STRUCT wvms
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	SIZE_T Bytes;
	if (g_TargetProcessInfo.ProcessStatus == TRUE)
	{
		Status = MmCopyVirtualMemory(PsGetCurrentProcess(), wvms->Value, g_TargetProcessInfo.Process, (PVOID)wvms->Address, (SIZE_T)wvms->Size, KernelMode, &Bytes);
	}
	return Status;
}