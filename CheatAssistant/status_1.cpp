#include "pch.h"
#include "status_1.h"
#include "function.h"
#include "role.h"
#include "task.h"
#include "send_packet.h"
#include "knapsac.h"

void status_1::manage()
{
	if (role::getCurrentRoleFatigueValue() <= g_预留疲劳 && g_自动模式 != 练习)
	{
		returnToRole();
		return;
	}
	if (g_自动模式 != 练习)
	{
		if (role::getRoleLevel() > 83 && wcscmp(role::getRoleJobName().c_str(), L"破晓女神") == 0)
		{
			g_自动模式 = 搬砖;
		}
	}
	//printf("model->:%d %d\n", g_自动模式, wcscmp(role::getRoleJobName().c_str(), L"破晓女神"));
	if (g_自动模式 == 搬砖)
	{
		if (role::getRoleLevel() > 83 && wcscmp(role::getRoleJobName().c_str(), L"破晓女神") == 0)
		{
			enterIntoCopy(g_副本编号, 1);
		}
		else {
			g_自动模式 = 剧情;
		}
		return;
	}
	else if (g_自动模式 == 剧情) {
		if (role::getRoleLevel() < 90)
		{
			task::autoMasterTask();
			return;
		}
		else {
			returnToRole();
			return;
		}
	}
	else if (g_自动模式 == 练习) {
		enterIntoCopy(g_副本编号, 1);
	}
	else {
		utils::myprintf(VMProtectDecryptStringA("没有合适的地图，自动关闭"),RED);
		g_自动开关 = false;
	}
}

void status_1::returnToRole()
{
	while (fun::getGameStatus() == 1 && g_自动开关)
	{
		key.doKeyPress(VK_ESCAPE);
		Sleep(1000);
		key.setMousePos(gameWindowInfo.left + 518, gameWindowInfo.top + 449);
		Sleep(1000);
		key.mouseClick();
		Sleep(3000);
	}
}

void status_1::enterIntoCopy(int copy_id,int model)
{
	ROLE_POS rolePos = role::getRolePos();
	POS room;
	utils::myprintf(VMProtectDecryptStringA("进入副本区域 副本ID %d"), PINK, copy_id);
	if (copy_id == 格蓝迪)
	{
		if (rolePos.room.x == 14 && rolePos.room.y == 2)
		{
			key.keyDown(VK_NUMPAD3);
			while (g_自动开关)
			{
				if (fun::getGameStatus() == 2)
				{
					key.keyUp(VK_NUMPAD3);
					return;
				}
				Sleep(500);
			}
		}
		else {
			room = {14,2};
			rolePos.room = room;
			rolePos.x = 705;
			rolePos.y = 288;
		}
	}
	utils::myprintf(VMProtectDecryptStringA("model %d rolePos.room.x %d rolePos.room.y %d rolePos.x %d rolePos.y %d"), PINK, model, rolePos.room.x , rolePos.room.y , rolePos.x , rolePos.y);
	if (model == 0)
	{
		role::moveRoleToPos(rolePos);
	}
	else if (model == 1) {
		SendPacket().城镇瞬移(rolePos, knapsac::getGoodsIndexByGoodsName(VMProtectDecryptStringW(L"瞬间移动药剂")));
		Sleep(1000);
	}
}
