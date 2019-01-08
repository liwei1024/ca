#include "pch.h"
#include "function.h"
#include "hook.h"
#include "role.h"
#include "call.h"
#include "send_packet.h"
#include "knapsac.h"
#include "task.h"

Hook messageHook;

void fun::remoteMainThreadCall(byte * opcodes, size_t opcodes_size, LPVOID param, size_t paramSize)
{
	/*int paramAddress = rw4.dwProcessBaseAddress + 1000;
	int callAddress = rw4.dwProcessBaseAddress + 1000 + (int)paramSize;*/
	std::vector<byte> oldData1;
	std::vector<byte> oldData2;
	oldData1 = memory.readBytes(__CALL地址, opcodes_size);
	if (param > 0 && paramSize > 0)
	{
		oldData2 = memory.readBytes(__CALL参数, opcodes_size);
		memory.writeVirtualMemory(__CALL参数, param, paramSize);
	}
	if (memory.writeVirtualMemory(__CALL地址, opcodes, opcodes_size) == TRUE) {
		SendMessage(g_hWnd, MY_MESSAGE_ID, __CALL地址, 0);
		if (param > 0 && paramSize > 0)
		{
			memory.writeBytes(__CALL参数, oldData2);
		}
		memory.writeBytes(__CALL地址, oldData1);
	}
	else {
		printf(VMProtectDecryptStringA("写内存地址失败\n"));
	}
}

//解密
int fun::decrypt(int address)
{
	int eax, esi, edx, i;
	eax = memory.read<int>(address);
	esi = memory.read<int>(__解密基址);
	edx = eax >> 16;
	edx = memory.read<int>(esi + edx * 4 + 36);
	eax = eax & 65535;
	eax = memory.read<int>(edx + eax * 4 + 8468);
	edx = eax & 65535;
	esi = edx << 16;
	esi = esi | edx;
	i = memory.read<int>(address + 4);
	esi = esi ^ i;
	return esi;
}
//加密
void fun::encrypt(INT32 Address, INT32 Value)
{
	INT32 EncryptId = 0;
	INT32 OffsetParam = 0;
	INT32 OffsetAddress = 0;
	INT32 Data = 0;
	INT32 AddressMask = 0;
	INT16 ax = 0;
	INT16 si = 0;
	EncryptId = memory.read<int>(Address);
	OffsetParam = memory.read<int>(memory.read<int>(__解密基址) + (EncryptId >> 16) * 4 + 36);
	OffsetAddress = OffsetParam + (EncryptId & 0xFFFF) * 4 + 8468;
	OffsetParam = memory.read<int>(OffsetAddress);
	Data = OffsetParam & 0xFFFF;
	Data += Data << 16;
	ax = OffsetParam & 0xFFFF;
	AddressMask = Address & 0xF;
	if (AddressMask == 0x0)
	{
		si = Value >> 16;
		si -= ax;
		si += Value;
	}
	else if (AddressMask == 0x4)
	{
		si = (Value & 0xFFFF) - (Value >> 16);
	}
	else if (AddressMask == 0x8)
	{
		si = Value >> 16;
		si *= Value;
	}
	else if (AddressMask == 0xC)
	{
		si = Value >> 16;
		si += Value;
		si += ax;
	}
	else
	{
		return;
	}
	ax ^= si;
	memory.write<BYTE>(OffsetAddress + 2, (BYTE)ax);
	memory.write<BYTE>(OffsetAddress + 3, (BYTE)(ax >> 8));
	memory.write<int>(Address + 4, Data ^ Value);
}

int fun::getGameStatus()
{
	return memory.read<int>(__游戏状态);
}
POS fun::getCurrentRoomPos()
{
	POS CurrentRoomPos;
	if (getGameStatus() == 1)
	{
		CurrentRoomPos.x = memory.readOffset<int>(__遍历取值, { __大区域偏移 });
		CurrentRoomPos.y = memory.readOffset<int>(__遍历取值, { __小区域偏移 });
	}
	else {
		DWORD OffsetAddress = memory.readOffset<int>(__房间编号, { __时间基址 ,__门型偏移 });
		CurrentRoomPos.x = memory.read<int>(OffsetAddress + __当前房间X);
		CurrentRoomPos.y = memory.read<int>(OffsetAddress + __当前房间Y);
	}

	return CurrentRoomPos;
}

POS fun::getBossRoomPos()
{
	POS BossRoomPos;
	DWORD OffsetAddress = memory.readOffset<int>(__房间编号, { __时间基址 ,__门型偏移 });
	BossRoomPos.x = decrypt(OffsetAddress + __BOSS房间X);
	BossRoomPos.y = decrypt(OffsetAddress + __BOSS房间Y);
	return BossRoomPos;
}

bool fun::isBossRoom()
{
	POS current_room_pos;
	POS boss_room_pos;

	current_room_pos = getCurrentRoomPos();
	boss_room_pos = getBossRoomPos();

	if (current_room_pos.x == boss_room_pos.x && current_room_pos.y == boss_room_pos.y)
	{
		return true;
	}
	return false;
}

bool fun::isOpenDoor()
{
	if (decrypt(memory.readOffset<int>(__人物基址, { __地图偏移 }) + __开门偏移) == 0)
	{
		return true;
	}
	else {
		return false;
	}
}

void fun::hookWindowMessage()
{
	int cross_core = (int)(DWORD_PTR)memory.getModuleHandleByModuleName(VMProtectDecryptStringW(L"cross_core.dll"));
	if (cross_core)
	{
		byte opcodes[] = {
			0x81,0x7d,0x0c,0x0,0x0,0x0,0x0,
			0x0f,0x85,0x0e,0x0,0x0,0x0,
			0x60,
			0x9c,
			0x8b,0x44,0x24,0x34,
			0xff,0xd0,
			0x9d,
			0x61,
			0x8b,0xe5,
			0x5d,
			0xc3,
			0x83,0xec,0x5c,
			0xa1,0x0,0x0,0x0,0x0,//33
			0xe9,0x0,0x0,0x0,0x0//38
		};

		*(int*)(opcodes + 3) = MY_MESSAGE_ID;
		*(int*)(opcodes + 31) = cross_core + 0x163A90;//
		*(int*)(opcodes + 36) = (cross_core + 0x5111E) - (sizeof(opcodes) - 5) - MESSAGE_HOOK_ADDRESS - 5;
		if (memory.writeVirtualMemory(MESSAGE_HOOK_ADDRESS, opcodes, sizeof(opcodes))) {
			messageHook.jmpHook(cross_core + 0x51116, MESSAGE_HOOK_ADDRESS,7);
		}

	}
}

void fun::unHookWindowMessage() 
{
	messageHook.unHook();
}

// 升级 自适应角色等级地图
void fun::chooseTheAppropriateMap(int taskId) 
{
	int roleLevel = role::getRoleLevel();
	int mapId = 0;
	CITY_INFO city_info;
	ROLE_POS rolePos = role::getRolePos();
	int copy_rand = 0;
	if (roleLevel == 62)
	{
		mapId = 7149;
		copy_rand = 0;
	}
	if (roleLevel == 53)
	{
		mapId = 7147;
		copy_rand = 0;
	}

	if (taskId > 0)
	{
		// 向罗杰汇报
		if (taskId == 3173) {
			mapId = 13;
			copy_rand = 0;
		}
		// 从天而落之物
		if (taskId == 3191) {
			mapId = 17;
			copy_rand = 1;
		}
		// 根特的守护神
		if (taskId == 3521) {
			mapId = 80;
			copy_rand = 1;
		}
		// 奇怪的兵器
		if (taskId == 3525) {
			mapId = 82;
			copy_rand = 0;
		}
		// 传染病治疗剂
		if (taskId == 3345)
		{
			mapId = 51;
			copy_rand = 1;
		}
		// 依然存在的不祥之感(二觉觉醒任务)
		if (taskId == 3413)
		{
			mapId = 7146;
			copy_rand = 0;
		}
		// 前往赫顿玛尔废墟
		if (taskId == 3451)
		{
			mapId = 7147;
			copy_rand = 0;
		}
		// 梅娅女王
		if (taskId == 3609)
		{
			mapId = 36;
			copy_rand = 0;
		}
		// 艾丽丝的帮助
		if (taskId == 3209)
		{
			mapId = 25;
			copy_rand = 0;
		}
	}

	utils::myprintf(VMProtectDecryptStringA("自适应的副本ID %d"),PINK, mapId);
	if (mapId > 0)
	{
		call::区域Call(&city_info, mapId);
		if (
			rolePos.room.x != city_info.room.x ||
			rolePos.room.y != city_info.room.y
			)
		{
			SendPacket().城镇瞬移(city_info, knapsac::getGoodsIndexByGoodsName(VMProtectDecryptStringW(L"瞬间移动药剂")));
			Sleep(1000);
		}
		SendPacket().进入选图();
		while (fun::getGameStatus() != 2)
		{
			Sleep(1000);
		}
		SendPacket().选择副本(mapId, copy_rand, 搬砖, task::getMissionTaskId());
		while (fun::getGameStatus() != 3)
		{
			Sleep(1000);
		}
	}
	else {
		utils::myprintf(VMProtectDecryptStringA("未获取到可适应的副本，自动关闭"));
		g_自动开关 = false;
	}
}

bool fun::exception(int exceptionCode) {
	
}

bool fun::passStoryFrame() {

	if (memory.read<int>(__对话基址2) == 1)
	{

		key.doKeyPress(VK_ESCAPE);
		key.doKeyPress(VK_SPACE);
		Sleep(500);
		return false;
	}
	return true;
}