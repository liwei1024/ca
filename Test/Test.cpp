#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include "../KernelCheatAssistan3/Packet.h"
#pragma comment(lib, "Ws2_32")

SOCKET connection;

void t1()
{
	Packet packet{ };
	int value = 0;
	int i = 0;
	int timeout = 3000; //3s
	int ret = setsockopt(connection, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	//int ret = setsockopt(connection, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	while (i < 20000)
	{
		packet.header.verify_code = verify_code;
		packet.header.type = PacketType::packet_read_memory;
		packet.data.read_memory.address = 0x01ADD570;
		packet.data.read_memory.size = sizeof(value);
		send(connection, (char *)&packet, sizeof(Packet), 0);
		recv(connection, (char*)&value, sizeof(value), 0);
		printf("%d  value->:%d\n",i, value);

		packet.header.verify_code = verify_code;
		packet.header.type = PacketType::packet_completed;
		recv(connection, (char *)&packet, sizeof(Packet), 0);
		printf("packet.data.completed.result->:%d\n", packet.data.completed.result);
		//Sleep(10);
		i++;
	}

}

int main()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	SOCKADDR_IN address{ };

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(server_ip);
	address.sin_port = htons(server_port);

	connection = socket(AF_INET, SOCK_STREAM, 0);
	/*if (connection == INVALID_SOCKET)
		return INVALID_SOCKET;*/

	if (connect(connection, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		closesocket(connection);
		//return INVALID_SOCKET;
	}

	Packet packet{ };
	uint64_t result = 0;

	packet.header.verify_code = verify_code;
	packet.header.type = PacketType::packet_get_target_process_status;

	
	send(connection, (char *)&packet, sizeof(Packet), 0);
	//return result;
	NTSTATUS status = 0;
	recv(connection, (char*)&status, sizeof(status), 0);
	printf("status->:%d\n", status);

	packet.header.verify_code = verify_code;
	packet.header.type = PacketType::packet_completed;
	recv(connection, (char *)&packet, sizeof(Packet), 0);
	printf("packet.data.completed.result->:%d\n", packet.data.completed.result);
	//return 0;
	//closesocket(connection);
	WaitForSingleObject(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)t1, NULL, 0, NULL), INFINITE);
	system("pause");
	return 0;
}

