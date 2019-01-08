#include "pch.h"
#include "role.h"
#include "function.h"
#include "status_3.h"


int role::getRoleLevel()
{
	return memory.read<int>(__角色等级);
}
std::wstring role::getRoleJobName()
{
	return memory.readWString(memory.read<DWORD>(__职业名称) + 0x0, 100);
}
int role::getCurrentRoleFatigueValue()
{
	return fun::decrypt(__最大疲劳) - fun::decrypt(__当前疲劳);
}
ROLE_POS role::getRolePos()
{
	ROLE_POS RolePos;
	RolePos.room = fun::getCurrentRoomPos();
	RolePos.x = (int)memory.read<float>(__角色坐标);
	RolePos.y = (int)memory.read<float>(__角色坐标 + 4);
	RolePos.z = (int)memory.read<float>(__角色坐标 + 8);
	return RolePos;
}
int role::getRoleStatus()
{
	return memory.read<int>(memory.read<int>(__角色状态) + __角色状态偏移);
}
// 去角色面向 0-左 1-右
int role::getRoleFacing() {
	if (memory.read<int>(memory.read<int>(__角色状态) + __角色面向偏移1) == 1 || memory.read<int>(memory.read<int>(__角色状态) + __角色面向偏移2) == 1) {
		return 1;
	}
	return 0;
}
void role::releaseSkillByKey(int keyCode, int s)
{
	if (fun::isOpenDoor() == false && status_3::getMonsterCount() > 0)
	{
		key.doKeyPress(keyCode, s);
		Sleep(100);
		// 释放技能的延时
		while (getRoleStatus() != 0)
		{
			Sleep(100);
		}
		Sleep(300);
	}
}

void role::moveRoleToPos_城镇(ROLE_POS targetPos)
{
	
}

void role::moveRoleToPos_过图(ROLE_POS targetPos)
{

}

void role::moveRoleToPos_捡物(ROLE_POS targetPos)
{
	ROLE_POS currentPos = getRolePos();
	std::vector<int> directionArray = moveRoleToPos_计算方向(currentPos, targetPos);
	if (directionArray[0] == 1)
	{
		key.keyDown(VK_NUMPAD1);
	}
	if (directionArray[1] == 1)
	{
		key.keyDown(VK_NUMPAD3);
	}
	if (directionArray[2] == 1)
	{
		key.keyDown(VK_NUMPAD5);
	}
	if (directionArray[3] == 1)
	{
		key.keyDown(VK_NUMPAD2);
	}
}

void role::moveRoleToPos_打怪(ROLE_POS targetPos)
{

}
// 0-左 1-右 2-上 3-下 
std::vector<int> role::moveRoleToPos_计算方向(ROLE_POS currentPos,ROLE_POS targetPos)
{
	std::vector<int> directionArray = {0,0,0,0};
	if (abs(currentPos.x - targetPos.x) > 50)
	{
		if (currentPos.x > targetPos.x)
		{
			directionArray[0] = 1;
		}
		else
		{
			directionArray[1] = 1;
		}
	}
	if (abs(currentPos.y - targetPos.y) > 20)
	{
		if (currentPos.y > targetPos.y)
		{
			directionArray[2] = 1;
		}
		else
		{
			directionArray[3] = 1;
		}
	}
	return directionArray;
}

void role::moveRoleToPos_卡点处理(ROLE_POS targetPos, std::map<const char*, bool> &cardPointList)
{
	ROLE_POS currentPos = getRolePos();
}


void role::moveRoleToPos(ROLE_POS targetPos,int type)
{
	// 时间变量
	int t1, t2 = (int)utils::getTime();
	// 游戏状态
	int gameStatus = fun::getGameStatus();
	//utils::myprintf("gameStatus->%d", GREEN, gameStatus);
	// 卡点列表
	//std::map<const char*,bool> cardPointList;
	// 位置变量
	ROLE_POS currentPos, cardPointPos = getRolePos();
	//utils::myprintf("目标位置 房间x->:%d,房间y->:%d | x->:%d,y->:%d", RED, targetPos.room.x, targetPos.room.y, targetPos.x, targetPos.y);
	while (true)
	{
		currentPos = getRolePos();
		t1 = utils::getTime();
		//utils::myprintf("当前位置 房间x->:%d,房间y->:%d | x->:%d,y->:%d", YELLOW, currentPos.room.x, currentPos.room.y, currentPos.x, currentPos.y);
		if (
			currentPos.room.x != targetPos.room.x ||
			currentPos.room.y != targetPos.room.y ||
			(abs(currentPos.x - targetPos.x) < 50 && abs(currentPos.y - targetPos.y) < 30)
			)
		{
			key.upAllKey();//还原所有按键
			//utils::myprintf("成功到达指定位置");
			break;
		}

		if ((currentPos.y - targetPos.y) >= 30 && key.getKeyState(VK_NUMPAD5) == 0)
		{
			key.keyDown(VK_NUMPAD5);
		}

		if ((targetPos.y - currentPos.y) >= 30 && key.getKeyState(VK_NUMPAD2) == 0)
		{
			key.keyDown(VK_NUMPAD2);
		}

		if ((currentPos.x - targetPos.x) >= 50 && key.getKeyState(VK_NUMPAD1) == 0)
		{
			//printf("左\n");
			if (gameStatus == 3)
			{
				key.keyDown(VK_NUMPAD1);
				Sleep(100);
				key.keyUp(VK_NUMPAD1);
				Sleep(100);
				key.keyDown(VK_NUMPAD1);
			}
			else {
				key.keyDown(VK_NUMPAD1);
			}
		}

		if ((targetPos.x - currentPos.x) >= 50 && key.getKeyState(VK_NUMPAD3) == 0)
		{
			//printf("右\n");
			if (gameStatus == 3)
			{
				key.keyDown(VK_NUMPAD3);
				Sleep(100);
				key.keyUp(VK_NUMPAD3);
				Sleep(100);
				key.keyDown(VK_NUMPAD3);
			}
			else {
				key.keyDown(VK_NUMPAD3);
			}
		}




		if (abs(currentPos.y - targetPos.y) < 30)
		{
			if (key.getKeyState(VK_NUMPAD5) == 1) {
				key.keyUp(VK_NUMPAD5);
				//utils::myprintf("keyUp 上");
			}
			if (key.getKeyState(VK_NUMPAD2) == 1)
			{
				key.keyUp(VK_NUMPAD2);
				//utils::myprintf("keyUp 下");
			}

		}

		if (abs(currentPos.x - targetPos.x) < 50)
		{
			if (key.getKeyState(VK_NUMPAD1) == 1) {
				key.keyUp(VK_NUMPAD1);
			}
			if (key.getKeyState(VK_NUMPAD3) == 1)
			{
				key.keyUp(VK_NUMPAD3);
			}
			//utils::myprintf("keyUp 左右");
		}

		// 卡点处理
		/*if (abs((int)t1 - (int)t2) > 3)
		{
			t2 = utils::getTime();
			if (
				abs(currentPos.x - cardPointPos.x) < 3 &&
				abs(currentPos.y - cardPointPos.y) < 3
				)
			{

				if (key.getKeyState(VK_NUMPAD1) == 1)
				{
					key.keyUp(VK_NUMPAD1);
					key.doKeyPress(VK_NUMPAD3, 500);
				}

				if (key.getKeyState(VK_NUMPAD2) == 1)
				{
					key.keyUp(VK_NUMPAD2);
					key.doKeyPress(VK_NUMPAD5, 500);
				}

				if (key.getKeyState(VK_NUMPAD3) == 1)
				{
					key.keyUp(VK_NUMPAD3);
					key.doKeyPress(VK_NUMPAD1, 500);
				}

				if (key.getKeyState(VK_NUMPAD5) == 1)
				{
					key.keyUp(VK_NUMPAD5);
					key.doKeyPress(VK_NUMPAD2, 500);
				}
			}
			cardPointPos = getRolePos();
		}*/
	}
}

// 取角色移动速度单位秒（）
int role::getRoleMoveSpeed()
{
	return 123;
}

void role::moveRoleToPos2(ROLE_POS targetPos, int type) 
{
	// 当前位置
	ROLE_POS currentPos = getRolePos();
	// 距离
	int xD = abs(targetPos.x - currentPos.x);
	int yD = abs(targetPos.y - currentPos.y);
	// 移动速度
	int xMS = 300;
	int yMS = 50;
	// 移动到目标位置所需要的时间(毫秒)
	int xT = int(xD / xMS) * 1000;
	int yT = int(yD / yMS) * 1000;

	// 往右走
	if (targetPos.x > currentPos.x)
	{
		key.doKeyPress(VK_NUMPAD3, xT);
	}
	// 往左走
	if (targetPos.x < currentPos.x)
	{
		key.doKeyPress(VK_NUMPAD1, xT);
	}
	// 往上走
	if (targetPos.y > currentPos.y)
	{
		key.doKeyPress(VK_NUMPAD5, yT);
	}
	// 往下走
	if (targetPos.y < currentPos.y)
	{
		key.doKeyPress(VK_NUMPAD2, yT);
	}
}


void role::ignoreBuild(bool enbale) {
	if (enbale == true)
	{
		memory.writeOffset(__人物基址, { __无视建筑偏移 }, 0);
		utils::myprintf(VMProtectDecryptStringA("无视建筑已开启"));
	}
	else {
		memory.writeOffset(__人物基址, { __无视建筑偏移 }, 40);
		utils::myprintf(VMProtectDecryptStringA("无视建筑已关闭"));
	}
}

void role::ignoreTerrain(bool enbale) {
	if (enbale == true)
	{
		memory.writeOffset(__人物基址, { __无视地形偏移 }, 0);
		utils::myprintf(VMProtectDecryptStringA("无视地形已开启"));
	}
	else {
		memory.writeOffset(__人物基址, { __无视地形偏移 }, 10);
		utils::myprintf(VMProtectDecryptStringA("无视地形已开启"));
	}
}