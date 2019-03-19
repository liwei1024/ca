#include <ntddk.h>
extern "C" {
	#include "ksocket/berkeley.h"
	#include "ksocket/ksocket.h"
	typedef int SOCKET;
	#define INVALID_SOCKET  (SOCKET)(-1)
	#define SOCKET_ERROR			(-1)
	DRIVER_INITIALIZE DriverEntry;

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

}
#include "Packet.h"

#define dprintf(Format, ...) DbgPrint("KCA: " Format "\n", __VA_ARGS__)

namespace Global {
	PEPROCESS TargetProcess;
	NTSTATUS TargetProcessStatus; //0 退出 1运行中
	HANDLE TargetProcessId;
	SOCKET ListenSocket;
	SOCKET ClientSocket;
	bool ThreadSwitch = true;
	KEVENT g_kEvent_t1;
	KEVENT g_kEvent_t2;
}

namespace Socket {

	void NTAPI ServerThread(void*);
	SOCKET CreateListenSocket();
	void NTAPI ConnectionThread(void* connection_socket);
	NTSTATUS PacketDispose(const SOCKET connection_socket, Packet packet);
	bool PacketCompleted(const SOCKET client_connection, NTSTATUS result);
}

namespace Notify {
	VOID CreateProcessNotifyEx(
		_Inout_  PEPROCESS              Process,
		_In_     HANDLE                 ProcessId,
		_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
	);

	VOID NotifyImageLoadCallback(
		_In_opt_ PUNICODE_STRING FullImageName,
		_In_ HANDLE ProcessId,                // pid into which image is being mapped
		_In_ PIMAGE_INFO ImageInfo
	);
}

namespace Driver {

	VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject);
}

namespace Fun {
	NTSTATUS ReadMemory(const SOCKET client_connection,PacketReadMemory packet);
}


NTSTATUS DriverEntry(
	_In_ struct _DRIVER_OBJECT *DriverObject,
	_In_ PUNICODE_STRING RegistryPath
){
	NTSTATUS Status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(RegistryPath);

	HANDLE thread_handle = nullptr;

	Status = PsSetCreateProcessNotifyRoutineEx(Notify::CreateProcessNotifyEx, FALSE);
	if (!NT_SUCCESS(Status))
	{
		//dprintf("Failed to create server thread. Status code: %X.", status);
		return STATUS_UNSUCCESSFUL;
	}

	Status = PsSetLoadImageNotifyRoutine(Notify::NotifyImageLoadCallback);

	if (!NT_SUCCESS(Status))
	{
		//dprintf("Failed to create server thread. Status code: %X.", status);
		return STATUS_UNSUCCESSFUL;
	}

	KeInitializeEvent(&Global::g_kEvent_t1, SynchronizationEvent, FALSE);
	Status = PsCreateSystemThread(
		&thread_handle,
		GENERIC_ALL,
		nullptr,
		nullptr,
		nullptr,
		Socket::ServerThread,
		nullptr
	);
	if (!NT_SUCCESS(Status))
	{
		//dprintf("Failed to create server thread. Status code: %X.", status);
		return STATUS_UNSUCCESSFUL;
	}
	ZwClose(thread_handle);

	DriverObject->DriverUnload = Driver::DriverUnload;
	return Status;
}

VOID Driver::DriverUnload(
	_In_ struct _DRIVER_OBJECT *DriverObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);

	PsRemoveLoadImageNotifyRoutine(Notify::NotifyImageLoadCallback);

	PsSetCreateProcessNotifyRoutineEx(Notify::CreateProcessNotifyEx, TRUE);

	Global::ThreadSwitch = false;

	closesocket(Global::ClientSocket); 
	KeWaitForSingleObject(
		&Global::g_kEvent_t2,
		Executive,
		KernelMode,
		FALSE,
		0         //再内核层的等待函数，0是永久等待
	);
	closesocket(Global::ListenSocket);
	KeWaitForSingleObject(
		&Global::g_kEvent_t1,
		Executive,
		KernelMode,
		FALSE,
		0         //再内核层的等待函数，0是永久等待
	);
}

void NTAPI Socket::ServerThread(void*)
{
	NTSTATUS Status = KsInitialize();
	if (!NT_SUCCESS(Status))
	{
		dprintf("Failed to initialize KSOCKET. Status code: %X.", Status);
		return;
	}


	Global::ListenSocket = Socket::CreateListenSocket();
	if (Global::ListenSocket == INVALID_SOCKET)
	{
		dprintf("Failed to initialize listening socket.");
		KsDestroy();
		return;
	}

	while (Global::ThreadSwitch)
	{
		sockaddr  socket_addr{ };
		socklen_t socket_length{ };

		const auto client_connection = accept(Global::ListenSocket, &socket_addr, &socket_length);
		if (client_connection == INVALID_SOCKET)
		{
			dprintf("Failed to accept client connection.");
			break;
		}

		HANDLE thread_handle = nullptr;

		// Create a thread that will handle connection with client.
		// TODO: Limit number of threads.
		KeInitializeEvent(&Global::g_kEvent_t2, SynchronizationEvent, FALSE);
		Status = PsCreateSystemThread(
			&thread_handle,
			GENERIC_ALL,
			nullptr,
			nullptr,
			nullptr,
			ConnectionThread,
			(void*)client_connection
		);

		if (!NT_SUCCESS(Status))
		{
			dprintf("Failed to create thread for handling client connection.");
			closesocket(client_connection);
			break;
		}

		ZwClose(thread_handle);
	}

	//closesocket(Global::ListenSocket);
	KeSetEvent(&Global::g_kEvent_t1, 0, TRUE);
	PsTerminateSystemThread(STATUS_SUCCESS);

}

// 创建监听socket
SOCKET Socket::CreateListenSocket()
{
	SOCKADDR_IN address{ };

	address.sin_family = AF_INET;
	address.sin_port = htons(server_port);

	SOCKET listen_socket = socket_listen(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		dprintf("Failed to create listen socket.");
		return INVALID_SOCKET;
	}

	if (bind(listen_socket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		dprintf("Failed to bind socket.");
		closesocket(listen_socket);
		return INVALID_SOCKET;
	}

	if (listen(listen_socket, 10) == SOCKET_ERROR)
	{
		dprintf("Failed to set socket mode to listening.");
		closesocket(listen_socket);
		return INVALID_SOCKET;
	}

	return listen_socket;
}
// socket 链接分发处理
void NTAPI Socket::ConnectionThread(void* connection_socket) 
{
	Global::ClientSocket = SOCKET(ULONG_PTR(connection_socket));

	Packet packet = {};

	while (Global::ThreadSwitch)
	{
		int result = ::recv(Global::ClientSocket,&packet,sizeof(packet),0);
		if (result <= 0)
			break;

		if (result < sizeof(PacketHeader))
			continue;

		if (packet.header.verify_code != verify_code)
			continue;
		if (!Socket::PacketCompleted(Global::ClientSocket, Socket::PacketDispose(Global::ClientSocket, packet))) {
			dprintf("fail !!!!!!!!!!!!!!!!!");
			break;
		}
	}
	KeSetEvent(&Global::g_kEvent_t2, 0, TRUE);
	PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS Socket::PacketDispose(const SOCKET connection_socket, Packet packet)
{
	NTSTATUS Status = STATUS_SUCCESS;
	switch (packet.header.type)
	{
	case PacketType::packet_read_memory:
		Status = Fun::ReadMemory(connection_socket, packet.data.read_memory);
		break;
	case PacketType::packet_get_target_process_status:
		Status = send(connection_socket,&Global::TargetProcessStatus,sizeof(Global::TargetProcessStatus),0)!= SOCKET_ERROR;
		break;
	default:
		break;
	}
	return NTSTATUS(Status);
}

bool Socket::PacketCompleted(const SOCKET client_connection, NTSTATUS result)
{
	Packet packet{ };

	packet.header.verify_code = verify_code;
	packet.header.type = PacketType::packet_completed;
	packet.data.completed.result = result;

	return (::send(client_connection, &packet, sizeof(packet), 0) != SOCKET_ERROR);
}

VOID Notify::CreateProcessNotifyEx(
	_Inout_  PEPROCESS              Process,
	_In_     HANDLE                 ProcessId,
	_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	if (NULL != CreateInfo)
	{
		if (wcsstr(CreateInfo->ImageFileName->Buffer, L"YoudaoDict.exe"))
		{
			dprintf("进程已经启动");
			Global::TargetProcess = Process;
			Global::TargetProcessId = ProcessId;
			Global::TargetProcessStatus = 1;
		}
		
	}
	else {
		if (Global::TargetProcessId == ProcessId)
		{
			Global::TargetProcessStatus = 0;
		}
	}
	
}

VOID Notify::NotifyImageLoadCallback(
	_In_opt_ PUNICODE_STRING FullImageName,
	_In_ HANDLE ProcessId,                // pid into which image is being mapped
	_In_ PIMAGE_INFO ImageInfo
)
{
	UNREFERENCED_PARAMETER(FullImageName);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ImageInfo);

	if (wcsstr(FullImageName->Buffer, L"DNF.exe") && wcsstr(FullImageName->Buffer, L"Device"))
	{
		/*g_TargetProcessInfo.MainThreadId = PsGetCurrentThreadId();
		g_TargetProcessInfo.MainThread = PsGetCurrentThread();
		g_TargetProcessInfo.ProcessBaseAddress = ImageInfo->ImageBase;*/
	}
}

NTSTATUS Fun::ReadMemory(const SOCKET client_connection, PacketReadMemory packet)
{
	NTSTATUS Status = STATUS_SUCCESS;

	PVOID  Buffer = ExAllocatePool(NonPagedPool,packet.size);
	RtlZeroMemory(Buffer, packet.size);
	SIZE_T ReturnSize = 0;

	Status = MmCopyVirtualMemory(
		Global::TargetProcess,
		(PVOID)packet.address,
		PsGetCurrentProcess(),
		Buffer,
		packet.size,
		KernelMode,
		&ReturnSize
	);

	::send(client_connection, Buffer, packet.size, 0);

	ExFreePool(Buffer);
	return Status;
}