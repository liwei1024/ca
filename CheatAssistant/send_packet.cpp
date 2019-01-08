#include "pch.h"
#include "send_packet.h"
#include "function.h"
//__asm {
//	push cdov
//	mov ecx, __发包基址
//	mov ecx, dword ptr ss : [ecx]
//	mov eax, __缓冲CALL
//	call eax
//}
void SendPacket::缓冲Call(int cdov) {
	byte bytes[] = {
		0x68,0x00,0x00,0x00,0x00,
		0xB9,0x00,0x00,0x00,0x00,
		0x8B,0x09,
		0xB8,0x00,0x00,0x00,0x00,
		0xFF,0xD0
	};
	*(int*)(bytes + 1) = cdov;
	*(int*)(bytes + 6) = __发包基址;
	*(int*)(bytes + 13) = __缓冲CALL;
	std::vector<byte> v_bytes = utils::bytesToVectorBytes(bytes, sizeof(bytes));
	v_shell_code.insert(v_shell_code.end(), v_bytes.begin(), v_bytes.end());

}
//__asm {
//	push wtud
//	mov ecx, __发包基址
//	mov ecx, dword ptr ss : [ecx]
//	mov eax, cdov
//	call eax
//}
void SendPacket::密包Call(int wtud, int taya) {
	int cdov = 0;
	if (taya == 1)cdov = __密包CALL;
	if (taya == 2)cdov = __密包CALL + 0x30;
	if (taya == 3)cdov = __密包CALL + 0x60;
	if (taya == 4)cdov = __密包CALL + 0x90;
	byte bytes[] = {
		0x68,0x00,0x00,0x00,0x00,
		0xB9,0x00,0x00,0x00,0x00,
		0x8B,0x09,
		0xB8,0x00,0x00,0x00,0x00,
		0xFF,0xD0
	};
	*(int*)(bytes + 1) = wtud;
	*(int*)(bytes + 6) = __发包基址;
	*(int*)(bytes + 13) = cdov;
	std::vector<byte> v_bytes = utils::bytesToVectorBytes(bytes, sizeof(bytes));
	v_shell_code.insert(v_shell_code.end(), v_bytes.begin(), v_bytes.end());
}
//__asm {
//	push wtud
//	mov ecx, __发包基址
//	mov ecx, dword ptr ss : [ecx]
//	mov eax, cdov
//	call eax
//}
void SendPacket::发包Call(bool is_send) {
	byte bytes[] = {
			0xB8,0x00,0x00,0x00,0x00,
			0xFF,0xD0,
			0x00
	};
	*(int*)(bytes + 1) = __发包CALL;
	if (is_send == true)*(byte*)(bytes + 7) = 0xC3;

	std::vector<byte> v_bytes = utils::bytesToVectorBytes(bytes, sizeof(bytes));
	v_shell_code.insert(v_shell_code.end(), v_bytes.begin(), v_bytes.end());
	if (is_send == true)
	{
		byte * shell_code = new byte[v_shell_code.size()];
		utils::vectorBytesToBytes(v_shell_code, shell_code);
		fun::remoteMainThreadCall(shell_code, v_shell_code.size());
		delete[]shell_code;
	}
}


void SendPacket::返回角色()
{
	缓冲Call(0x7);
	发包Call();
}

void SendPacket::选择角色(int role_index)
{
	缓冲Call(4);
	密包Call(role_index, 3);
	发包Call();
}

void SendPacket::城镇瞬移(CITY_INFO cityInfo, int goods_index)
{
	if (goods_index > 0)
	{
		缓冲Call(237);
		密包Call(goods_index, 2);
		密包Call(2600014, 3);
		密包Call(0, 1);
		密包Call(cityInfo.room.x, 1);
		密包Call(cityInfo.room.y, 1);
		密包Call(cityInfo.x, 2);
		密包Call(cityInfo.y, 2);
		发包Call();
	}
	else {
		缓冲Call(36);
		密包Call(cityInfo.room.x, 1);
		密包Call(cityInfo.room.y, 1);
		密包Call(cityInfo.x, 2);
		密包Call(cityInfo.y, 2);
		密包Call(5, 1);
		密包Call(38, 2);
		密包Call(1, 2);
		密包Call(0, 3);
		密包Call(0, 1);
		发包Call();
	}
}

void SendPacket::进入选图()
{
	缓冲Call(15);
	密包Call(0, 3);
	发包Call();
}

void SendPacket::选择副本(int 副本编号, int 副本难度, int 副本模式,int 任务ID)
{

	缓冲Call(16);
	密包Call(副本编号, 3);
	if (副本编号 > 7100 && 副本编号 < 7200)
	{
		密包Call(0, 1);
		密包Call(0, 2);
	}
	else
	{
		密包Call(副本难度, 1);
		密包Call(0, 2);
	}
	if (副本模式 == 深渊) { // 深渊
		密包Call(1, 1);
		密包Call(0, 1);
	}
	else if (副本模式 == 练习) {
		密包Call(0, 1);//练习
		密包Call(1, 1);
	}
	else {//普通
		密包Call(0, 1);
		密包Call(0, 1);
	}
	密包Call(65535, 2);
	密包Call(0, 3);
	密包Call(0, 1);
	if (副本编号 > 7100 && 副本编号 < 7200)
	{
		密包Call(0, 3);
	}
	else {
		密包Call(任务ID, 3);
	}
	发包Call();
}

void SendPacket::组包拾取(int loot_address, int x, int y)
{
	缓冲Call(43);
	密包Call(loot_address, 3);
	密包Call(0, 1);
	密包Call(1, 1);
	密包Call(x, 2);
	密包Call(y, 2);
	密包Call(0, 2);
	密包Call(x + ((x % 4) + 1), 2);
	密包Call(y + ((y % 4) + 1), 2);
	密包Call(0, 2);
	密包Call(0, 2);
	发包Call();
}

void SendPacket::组包翻牌()
{
	缓冲Call(69);
	发包Call();

	缓冲Call(70);
	发包Call();

	缓冲Call(71);
	密包Call(0, 1);
	发包Call();

	缓冲Call(1431);
	发包Call();
}

void SendPacket::组包卖物(int 物品下标)
{
	缓冲Call(593);
	密包Call(2, 3);
	密包Call(0, 3);
	发包Call();

	缓冲Call(22);
	密包Call(0, 1);
	密包Call(物品下标, 2);
	密包Call(1, 3);
	密包Call(1000, 3);
	密包Call(物品下标 * 2 + 3, 3);
	发包Call();
}


//void SendPacket::组包回城()
//{
//	using namespace utils;
//	缓冲Call(123);
//	密包Call(createRandom(1, 1500), 3);
//	密包Call(createRandom(1, 1500), 3);
//	密包Call(createRandom(1, 1500), 3);
//	密包Call(createRandom(1, 1500), 3);
//	密包Call(createRandom(1, 1500), 3);
//	密包Call(createRandom(1, 1500), 3);
//	发包Call();
//	缓冲Call(42);
//	发包Call();
//}