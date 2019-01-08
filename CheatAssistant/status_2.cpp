#include "pch.h"
#include "status_2.h"
#include "role.h"
#include "function.h"
#include "call.h"
#include "send_packet.h"

void status_2::manage() {
	if (role::getCurrentRoleFatigueValue() == 0)
	{
		key.doKeyPress(VK_ESCAPE);
		Sleep(1000);
	}
	else {
		if (g_自动模式 == 搬砖)
		{
			selectCopyByKey(g_副本编号, g_副本难度);
		}
		else if (g_自动模式 == 练习) {
			SendPacket().选择副本(g_副本编号, g_副本难度, g_自动模式, 0);
			while (fun::getGameStatus() != 3 && g_自动开关)
			{
				Sleep(1000);
			}
			Sleep(1000);
		}
		else {
			key.doKeyPress(VK_ESCAPE);
			Sleep(1000);
		}
	}
}

void status_2::selectCopyByKey(int 副本ID, int 副本难度)
{
	int 当前副本ID, 当前副本难度;
	int flag = true;
	if (fun::passStoryFrame() == false)
	{
		return;
	}
	while (flag && fun::getGameStatus() == 2)
	{
		当前副本ID = memory.read<int>(__副本ID);
		当前副本难度 = memory.read<int>(memory.read<int>(__副本难度) + __副本难度偏移);
		Sleep(500);
		if (副本ID == 当前副本ID && 当前副本难度 == 副本难度)
		{
			flag = false;
			while (g_自动开关)
			{
				if (fun::getGameStatus() == 3)
				{
					break;
				}
				key.doKeyPress(VK_SPACE);
				Sleep(3000);
			}
			fun::passStoryFrame();
			break;
		}
		if (副本ID != 当前副本ID)
		{
			key.doKeyPress(VK_NUMPAD2);
			Sleep(500);
			continue;
		}
		if (副本难度 != 当前副本难度 && 副本难度 > 当前副本难度)
		{
			key.doKeyPress(VK_NUMPAD3);
			Sleep(500);
			continue;
		}
		else if (副本难度 != 当前副本难度 && 副本难度 < 当前副本难度)
		{
			key.doKeyPress(VK_NUMPAD1);
			Sleep(500);
			continue;
		}
	}
}