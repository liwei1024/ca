#pragma once
constexpr auto verify_code = 19951024;
constexpr auto server_ip   = 0x7F000001; // 127.0.0.1
constexpr auto server_port = 27015;

enum class PacketType
{
	packet_read_memory,
	packet_completed,
	packet_get_target_process_status
};

struct PacketHeader
{
	NTSTATUS	verify_code;
	PacketType  type;
};

struct PacketReadMemory
{
	ULONG  address;
	SIZE_T size;
};

struct PackedCompleted
{
	NTSTATUS result;
};

struct Packet
{
	PacketHeader header;
	union 
	{
		PacketReadMemory read_memory;
		PackedCompleted  completed;
	}data;
};