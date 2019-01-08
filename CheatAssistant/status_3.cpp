#include "pch.h"
#include "function.h"
#include "status_3.h"
#include "role.h"
#include "astar.hpp"
#include "knapsac.h"
#include "call.h"
#include "task.h"

int moveToNextRoomFaulureNumber = 0;//过图失败计次
int getTheSpoilsFaulureNumber = 0;//捡物失败计次
void status_3::manage()
{
	int gameStatus = fun::getGameStatus();
	if (status_3::getCurrentCopyId() == -1)
	{
		return;
	}
	if (fun::isOpenDoor())
	{
		//utils::myprintf(VMProtectDecryptStringA("门已开"));
		if (g_自动模式 == 搬砖 && knapsac::getGoodsCount() < 15)
		{
			if (getTheSpoils() == true && getTheSpoilsFaulureNumber < 10)
			{
				getTheSpoilsFaulureNumber++;
				return;
			}
			getTheSpoilsFaulureNumber = 0;
			//utils::myprintf(VMProtectDecryptStringA("物品拾取完毕"));
		}
		if (fun::isBossRoom() == true && getMonsterCount() == 0)
		{
			switch (g_自动模式)
			{
			case 练习:
				while (g_自动开关)
				{
					if (gameStatus != fun::getGameStatus() || fun::isBossRoom() == false)
					{
						g_刷图次数++;
						g_过图时间 = utils::getTime() - g_过图时间;
						g_首图标记 = true;
						utils::mywprintf(VMProtectDecryptStringW(L"练习第 %d 次 耗时 %d s "), CYAN, g_刷图次数, (int)(g_过图时间 / 1000));
						Sleep(1000);
						break;
					}
					key.doKeyPress(VK_F10);
					Sleep(1000);
				}
				break;
			case 搬砖:
				while (g_自动开关)
				{
					if (getObjectPointerByCode(Code_通关营火) == NULL)
					{
						Sleep(2000);
						key.doKeyPress(VK_ESCAPE);
						continue;
					}
					//按键卖物();
					knapsac::keyPadSellThings();
					key.doKeyPress(VK_ESCAPE);
					while (g_自动开关)
					{
						if (gameStatus != fun::getGameStatus() || fun::isBossRoom() == false)
						{
							g_刷图次数++;
							g_过图时间 = utils::getTime() - g_过图时间;
							g_首图标记 = true;
							utils::mywprintf(VMProtectDecryptStringW(L"搬砖第 %d 次 耗时 %d 秒 "), CYAN, g_刷图次数, (int)(g_过图时间 / 1000));
							Sleep(500);
							break;
						}
						if (memory.read<int>(__对话基址2) == 1) {
							key.doKeyPress(VK_ESCAPE);
							key.doKeyPress(VK_SPACE);
							continue;
						}
						if (role::getCurrentRoleFatigueValue() <= 0) {
							key.doKeyPress(VK_F12);
						}
						else {
							key.doKeyPress(VK_F10);
						}
						Sleep(1000);
					}
					break;
				}
				break;
			case 剧情:
				while (g_自动开关)
				{
					if (getObjectPointerByCode(Code_通关营火) == NULL)
					{
						Sleep(2000);
						key.doKeyPress(VK_ESCAPE);
						continue;
					}
					
					if (task::isThearMainTask())
					{
						Sleep(1000);
					}
					
					key.doKeyPress(VK_V);
					while (g_自动开关)
					{
						if (gameStatus != fun::getGameStatus() || fun::isBossRoom() == false)
						{
							g_刷图次数++;
							g_过图时间 = utils::getTime() - g_过图时间;
							g_首图标记 = true;
							utils::mywprintf(VMProtectDecryptStringW(L"剧情第 %d 次 耗时 %d 秒 "), CYAN, g_刷图次数, (int)(g_过图时间 / 1000));
							Sleep(1000);
							break;
						}
						
						if (task::isThearMainTask())
						{
							key.doKeyPress(VK_ESCAPE);
							key.doKeyPress(VK_SPACE);
							key.doKeyPress(VK_ESCAPE);
							key.doKeyPress(VK_SPACE);
						}
						
						Sleep(1000);
						/*if (fun::passStoryFrame() == false)
						{
							continue;
						}*/
						if (getTheSpoilsCount() > 0)
						{
							//printf("getTheSpoilsCount->:%d\n", getTheSpoilsCount());
							key.doKeyPress(VK_X, (150 * getTheSpoilsCount()));
						}
						if (knapsac::getGoodsCount() > 13)
						{
							knapsac::keyPadSellThings();
							key.doKeyPress(VK_ESCAPE);
						}
						if (role::getCurrentRoleFatigueValue() <= 0 || task::currentMainTaskIsCanIgnore() == true || task::isThearMainTask() == true) {
							key.doKeyPress(VK_F12);
						}
						else {
							key.doKeyPress(VK_F10);
							Sleep(1000);
							key.doKeyPress(VK_F10);
							key.doKeyPress(VK_F12);
						}
						Sleep(1000);
					}
					break;
				}
				break;
			default:
				break;
			}
		}
		else {
			moveToNextRoom();
		}
	}
	else {
		//utils::myprintf(VMProtectDecryptStringA("开始打怪"));
		if (g_首图标记 == true)
		{
			g_过图时间 = utils::getTime();
			g_首图标记 = false;
		}
		if (fun::passStoryFrame() == false)
		{
			/*key.doKeyPress(VK_ESCAPE);
			Sleep(100);
			if (fun::passStoryFrame() == false)
			{
				key.doKeyPress(VK_RETURN);
			}*/
			return;
		}
		if (g_自动模式 == 剧情)
		{
			if (g_剧情功能 == 0)
			{
				if (g_变身代码 == 110525) {
					按键_蔡依林();
				}
				else if (g_变身代码 == 107000902) {
					按键_吞噬魔();
				}
			}
			else if (g_剧情功能 == 1) {
				follow();
				fullScreenSkills();
			}
			
		}
		else if (g_自动模式 == 搬砖 || g_自动模式 == 练习) {
			std::wstring role_job_name = role::getRoleJobName();
			if (wcscmp(role_job_name.c_str(), VMProtectDecryptStringW(L"破晓女神")) == 0)
			{
				按键_破晓女神();
			}
			/*else if (wcscmp(role_job_name.c_str(), L"帝血弑天") == 0) {
				按键_帝血弑天();
			}
			else if (wcscmp(role_job_name.c_str(), L"天帝") == 0) {
				按键_天帝();
			}
			else if (wcscmp(role_job_name.c_str(), L"剑神") == 0) {
				按键_剑神();
			}
			else if (wcscmp(role_job_name.c_str(), L"风神") == 0) {
				按键_风神();
			}*/
		}
		
	}
}

void status_3::moveToNextRoom()
{
	DWORD temp_data;
	DWORD coordinate_struct;
	ROLE_POS rolePos;
	AStarMapInfo map_info;
	int direction = getDirection(1);//GetDirection(1);//下个房间反向
	int currentCopyId = getCurrentCopyId();
	POS currentRoomPos = fun::getCurrentRoomPos();
	if (currentCopyId == 9)
	{
		if (currentRoomPos.x == 0 && currentRoomPos.y == 1)
		{
			direction = 2;
		}
		else if (currentRoomPos.x == 0 && currentRoomPos.y == 0) {
			direction = 1;
		}
	}
	/*else if (currentCopyId == 104) {
		if (currentRoomPos.x == 2 && currentRoomPos.y == 1)
		{
			direction = 0;
		}
		else if (currentRoomPos.x == 1 && currentRoomPos.y == 1) {
			direction = 3;
		}
	}*/
	//utils::myprintf("direction->:%x", RED, direction);
	int x, y, xf, yf, cx, cy = 0;
	temp_data = memory.read<DWORD>(__商店基址 - 8);
	temp_data = memory.read<DWORD>(temp_data + __时间基址);
	temp_data = memory.read<DWORD>(temp_data + __坐标结构偏移1);
	coordinate_struct = temp_data + (direction + direction * 8) * 4 + __坐标结构偏移2 + (direction * 4);
	//utils::myprintf(VMProtectDecryptStringA("coordinate_struct->:%x"),RED, coordinate_struct);
	x = memory.read<int>(coordinate_struct + 0x0);
	y = memory.read<int>(coordinate_struct + 0x4);
	xf = memory.read<int>(coordinate_struct + 0x8);
	yf = memory.read<int>(coordinate_struct + 0xc);
	if (direction == 0)
	{
		cx = x + xf + 20;
		cy = y + (yf / 2);
	}
	else if (direction == 1)
	{
		cx = x - 20;
		cy = y + (yf / 2);
	}
	else if (direction == 2)
	{
		cx = x + xf / 2;
		cy = y + yf + 20;
	}
	else if (direction == 3)
	{
		cx = x + xf / 2;
		cy = y - 20;
	}
	Sleep(200);
	rolePos = role::getRolePos();
	rolePos.x = cx;
	rolePos.y = cy;
	//utils::myprintf(VMProtectDecryptStringA("门坐标 x->:%d,y->:%d | xf->:%d,yf->:%d | cx->:%d,cy->:%d"), YELLOW, x, y, xf, yf, cx, cy);
	//utils::myprintf("门坐标 rolePos.x->:%d,rolePos.y->:%d", YELLOW, rolePos.x, rolePos.y);
	//Sleep(1000);
	moveRoleToPos((x + xf / 2), cy);
	if (direction == 0) {
		key.doKeyPress(VK_NUMPAD1, 300);
	}
	else if (direction == 1) {
		key.doKeyPress(VK_NUMPAD3, 300);
	}
	else if (direction == 2)
	{
		key.doKeyPress(VK_NUMPAD5, 300);
	}
	else if (direction == 3) {
		key.doKeyPress(VK_NUMPAD2, 300);
	}
	Sleep(200);
	if (rolePos.room.x == role::getRolePos().room.x && rolePos.room.y == role::getRolePos().room.y)
	{
		if (moveToNextRoomFaulureNumber>=2)
		{
			call::过图Call(direction);
		}
		moveToNextRoomFaulureNumber++;
	}
	else {
		moveToNextRoomFaulureNumber = 0;
	}
	
}

DWORD status_3::getMapAddress()
{

	return memory.readOffset<int>(__人物基址, { __地图偏移 });
}

DWORD status_3::getMapStartAddress()
{
	return  memory.read<int>(getMapAddress() + __首地址);
}

int status_3::getMapObjectCount(DWORD map_start_address)
{
	return  (memory.read<int>(getMapAddress() + __尾地址) - map_start_address) / 4;
}

void status_3::moveRoleToPos(int x,int y) 
{
	int pointer = 0;
	ROLE_POS currentPos;
	if (g_自动模式 == 剧情)
	{
		if (g_剧情功能 == 0)
		{
			MAP_OBJECT_STRUCT object;
			if (getObjectInfoByObjectCode(&object, g_变身代码) == TRUE) {
				currentPos.x = object.x;
				currentPos.y = object.y;
				pointer = object.address;
			}
			else {
				/*g_自动开关 = false;
				utils::myprintf(VMProtectDecryptStringA("剧情模式中 使用变身功能 但未获取到变身后的指针"));
				return;*/
				pointer = memory.read<int>(__人物基址);
			}
			
		}
	}
	if(pointer == 0) {
		currentPos = role::getRolePos();
		pointer = memory.read<int>(__人物基址);
	}
	if (g_移动方式 == 0) //坐标call
	{
		call::坐标Call(pointer,x,y,0);
	}
	else if (g_移动方式 == 1) { //脚本移动
		call::移动Call(pointer, x, y);
		Sleep((abs(currentPos.x - x) + abs(currentPos.y - y)) + 100);
	}
	Sleep(100);
}

int status_3::getCurrentCopyId()
{
	return memory.read<int>(__图内副本ID);
}
std::wstring status_3::getCurrentCopyName()
{
	return memory.readWString(__图内副本名称,100);
}
// 获取对象信息
MAP_OBJECT_STRUCT status_3::getObjectInfo(DWORD object_pointer)
{
	MAP_OBJECT_STRUCT object;
	DWORD pos_pointer;
	object.address = object_pointer;
	object.type = memory.read<DWORD>(object_pointer + __类型偏移);
	object.camp = memory.read<DWORD>(object_pointer + __阵营偏移);
	object.health_point = memory.read<DWORD>(object_pointer + __血量偏移);
	object.code = memory.read<DWORD>(object_pointer + __代码偏移);
	if (object.type == 289 && object.camp == 200)
	{
		object.name = memory.readWString(memory.read<DWORD>(memory.read<DWORD>(object_pointer + __地面物品名称偏移) + 0x24) + 0, 100);
	}
	else {
		object.name = memory.readWString(memory.read<DWORD>(object_pointer + __名称偏移), 100);
	}
	if (object.type == 273)
	{
		if (object_pointer == memory.read<DWORD>(__人物基址))
		{
			object.x = (int)memory.read<float>(__角色坐标 + 0);
			object.y = (int)memory.read<float>(__角色坐标 + 4);
			object.z = (int)memory.read<float>(__角色坐标 + 8);
		}
		else {
			pos_pointer = memory.read<int>(object_pointer + __人物怪坐标偏移);
			object.x = (int)memory.read<float>(pos_pointer + 0);
			object.y = (int)memory.read<float>(pos_pointer + 4);
			object.z = (int)memory.read<float>(pos_pointer + 8);
		}
	}
	else {
		pos_pointer = memory.read<int>(object_pointer + __坐标偏移);
		object.x = (int)memory.read<float>(pos_pointer + 0x10);
		object.y = (int)memory.read<float>(pos_pointer + 0x14);
		object.z = (int)memory.read<float>(pos_pointer + 0x18);
	}
	return object;
}
// 打印副本对象信息
void status_3::outputMapObjectInfo()
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	utils::myprintf(VMProtectDecryptStringA("map_start_address %x\n"), PINK, mapStartAddress);
	utils::myprintf(VMProtectDecryptStringA("map_object_count %d\n"), PINK, mapObjectCount);
	MAP_OBJECT_STRUCT _ObjectInfo;
	
	DWORD objectAddress;
	for (size_t i = 0; i < mapObjectCount; i++)
	{
		objectAddress = memory.read<int>((ULONG)(mapStartAddress + i * 4));
		if (objectAddress <= 0)continue;
		_ObjectInfo = getObjectInfo(objectAddress);
		utils::myprintf(VMProtectDecryptStringA("address 0x%x"), RED, _ObjectInfo.address);
		utils::myprintf(VMProtectDecryptStringA("code %d"), RED, _ObjectInfo.code);
		utils::myprintf(VMProtectDecryptStringA("type %d"), RED, _ObjectInfo.type);
		utils::myprintf(VMProtectDecryptStringA("camp %d"), RED, _ObjectInfo.camp);
		utils::myprintf(VMProtectDecryptStringA("health_point %d"), RED, _ObjectInfo.health_point);
		utils::myprintf(VMProtectDecryptStringA("pos %d,%d,%d"), RED, _ObjectInfo.x, _ObjectInfo.y, _ObjectInfo.z);
		utils::mywprintf(VMProtectDecryptStringW(L"name %s"), RED, _ObjectInfo.name.c_str());
		utils::myprintf(VMProtectDecryptStringA("====================================="));
	}
	utils::myprintf(VMProtectDecryptStringA("当前副本ID %d\n"), PINK, getCurrentCopyId());
	utils::mywprintf(VMProtectDecryptStringW(L"当前副本名称 %ws\n"), PINK, getCurrentCopyName().c_str());
	POS currentRoomPos = fun::getCurrentRoomPos();
	utils::mywprintf(VMProtectDecryptStringW(L"当前房间坐标 %d,%d\n"), PINK, currentRoomPos.x, currentRoomPos.y);
}
// 获取副本内怪物数量
int status_3::getMonsterCount()
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	std::vector<MAP_OBJECT_STRUCT> Objects;
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	int monsterCount = 0;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<DWORD>(mapStartAddress + i * 4);
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (g_自动模式 == 剧情 && object.name.find(L"假Boss") != -1)
		{
			continue;
		}
		if (object.code == 258 || object.code == 818 || object.code == 63821)
		{
			continue;
		}
		if (object.type == 529 || object.type == 273 || object.type == 545)
		{
			if (object.camp > 0)
			{
				if (object.health_point > 0 || object.code == 8104 || object.code == 817)
				{
					monsterCount++;
				}
			}
		}
	}
	return monsterCount;
}
// 全屏技能
void status_3::fullScreenSkills()
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	std::vector<MAP_OBJECT_STRUCT> Objects;
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	int rolePointer = memory.read<int>(__人物基址);
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<DWORD>(mapStartAddress + i * 4);
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (g_自动模式 == 剧情 && object.name.find(L"假Boss") != -1)
		{
			continue;
		}
		if (object.code == 258 || object.code == 818 || object.code == 63821)
		{
			continue;
		}
		if (object.type == 529 || object.type == 273 || object.type == 545)
		{
			if (object.camp > 0)
			{
				if (object.health_point > 0 || object.code == 8104 || object.code == 817)
				{
					call::技能Call(rolePointer, 48611, 0x113, object.x, object.y, object.z);
					Sleep(50);
				}
			}
		}
	}
}
// 查找是否有怪物Z轴大于35
bool status_3::findMonsterZ_AxisMoreThanThe35()
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	std::vector<MAP_OBJECT_STRUCT> Objects;
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	int monsterCount = 0;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<DWORD>(mapStartAddress + i * 4);
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (object.code == 258 || object.code == 818 || object.code == 63821)
		{
			continue;
		}
		/*if (getCurrentCopyId() == 148)
		{
			return true;
		}*/
		if (object.type == 529 || object.type == 273 || object.type == 545)
		{
			if (object.camp > 0)
			{
				if (object.health_point > 0 || object.code == 8104 || object.code == 817)
				{
					if (object.z >= 35)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
// 按怪物名称模糊查找
bool status_3::blurryFindMonsterByString(std::wstring string) {
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	std::vector<MAP_OBJECT_STRUCT> Objects;
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	int monsterCount = 0;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<DWORD>(mapStartAddress + i * 4);
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (object.code == 258 || object.code == 818 || object.code == 63821)
		{
			continue;
		}
		if (object.type == 529 || object.type == 273 || object.type == 545)
		{
			if (object.camp > 0)
			{
				if (object.health_point > 0 || object.code == 8104 || object.code == 817)
				{
					if (!(object.name.find(string.c_str(), 0) == -1)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}
// 按角色最近距离排序
void status_3::sortByDistance(std::vector<MAP_OBJECT_STRUCT> &Objects)
{
	MAP_OBJECT_STRUCT temp_var;
	ROLE_POS RolePos = role::getRolePos();
	for (size_t i = 0; i < Objects.size(); i++)
	{
		for (size_t j = 0; j < Objects.size() - i; j++)
		{
			if (abs(Objects[j].x - RolePos.x) + abs(Objects[j].y - RolePos.y) > abs(Objects[j + 1].x - RolePos.x) + abs(Objects[j + 1].y - RolePos.y))
			{
				temp_var = Objects[j];
				Objects[j + 1] = Objects[j];
				Objects[j] = temp_var;
			}
		}
	}
}
// 跟随
void status_3::follow(std::wstring name)
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	MAP_OBJECT_STRUCT object;
	ROLE_POS rolePos = role::getRolePos();
	DWORD objectAddress;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<int>(mapStartAddress + i * 4);
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (object.code == 258 || object.code == 818 || object.code == 63821)
			continue;
		if (!(object.health_point > 0 || object.code == 8104 || object.code == 817))
			continue;
		if (!name.empty() && object.name.find(name.c_str(), 0) == -1)
			continue;
		if (!(object.type == 529 || object.type == 273 || object.type == 545))
			continue;
		if (!(object.camp > 0))
			continue;
		if (g_自动模式 == 剧情 && wcsstr(object.name.c_str(),L"领主 - 暴戾搜捕团追踪者"))
		{
			continue;
		}
		if (abs(rolePos.x - object.x) > 200 || abs(rolePos.y - object.y) > 50 || wcsstr(object.name.c_str(),L"巨人波图拉") == 0)
		{
			if (rolePos.x > object.x)
			{
				moveRoleToPos(object.x + utils::createRandom(-10, 10) + 200, object.y + utils::createRandom(-10, 10));
				/*if (role::getRoleFacing() == 1)
				{*/
					key.doKeyPress(VK_NUMPAD1);
				//}
				
			}
			else {
				moveRoleToPos(object.x + utils::createRandom(-10, 10) - 200, object.y + utils::createRandom(-10, 10));
				/*if (role::getRoleFacing() == 0)
				{*/
					key.doKeyPress(VK_NUMPAD3);
				//}
			}
			Sleep(200);
			break;
		}
		else {
			if (rolePos.x > object.x) {
				/*if (role::getRoleFacing() == 1)
				{*/
				key.doKeyPress(VK_NUMPAD1);
				//}
			}
			else if (rolePos.x < object.x) {
				/*if (role::getRoleFacing() == 0)
				{*/
				key.doKeyPress(VK_NUMPAD3);
				//}
			}
		}
	}
}
// 变身打怪 吞噬魔
void status_3::按键_吞噬魔()
{
	MAP_OBJECT_STRUCT object;
	if (getObjectInfoByObjectCode(&object, g_变身代码) == FALSE)
	{
		call::变身Call(g_变身代码);
		while (g_自动开关) {
			if (getObjectInfoByObjectCode(&object, g_变身代码) == TRUE)
			{
				break;
			}
			Sleep(1000);
		}
	}
	if (findMonsterZ_AxisMoreThanThe35())
	{
		role::releaseSkillByKey(VK_F, 2500);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		follow();
		role::releaseSkillByKey(VK_A, 2000);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
	}
	else {
		follow();
		role::releaseSkillByKey(VK_S, 1000);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		follow();
		role::releaseSkillByKey(VK_S, 1000);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
	}
	if (blurryFindMonsterByString(VMProtectDecryptStringW(L"领主")) == true)
	{
		follow(VMProtectDecryptStringW(L"领主"));
		role::releaseSkillByKey(VK_D, 2000);
	}
	
}
// 变身打怪 蔡依林
void status_3::按键_蔡依林()
{
	MAP_OBJECT_STRUCT object;
	if (getObjectInfoByObjectCode(&object, g_变身代码) == FALSE)
	{
		call::变身Call(g_变身代码);
		Sleep(3000);
	}
	role::releaseSkillByKey(VK_A,1000);
	if (fun::isOpenDoor() == true)
	{
		return;
	}
	follow();
	role::releaseSkillByKey(VK_S, 1000);
	if (fun::isOpenDoor() == true)
	{
		return;
	}
	follow();
	role::releaseSkillByKey(VK_A, 1000);
	if (fun::isOpenDoor() == true)
	{
		return;
	}
	follow();
	role::releaseSkillByKey(VK_S, 1000);
	if (fun::isOpenDoor() == true)
	{
		return;
	}
	follow();
	role::releaseSkillByKey(VK_D, 1000);
}
// 根据对象代码获取对象信息
BOOL status_3::getObjectInfoByObjectCode(PMAP_OBJECT_STRUCT object,int objectCode)
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	ROLE_POS rolePos = role::getRolePos();
	DWORD objectAddress;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<int>(mapStartAddress + i * 4);
		if (objectAddress <= 0)continue;
		*object = getObjectInfo(objectAddress);
		if (object->code == objectCode)
		{
			return TRUE;
		}
	}
	return NULL;
}
// 获取战利品数量
int status_3::getTheSpoilsCount() {
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	ROLE_POS rolePos = role::getRolePos();
	int spoilsCount = 0;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<DWORD>((mapStartAddress + i * 4));
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (object.code == Code_鸡腿 || object.code == Code_肉块 || object.code == Code_成长之泉水)
			continue;
		if (object.type == 289 && object.camp == 200)
		{
			if (
				wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"金币")) != 0 &&
				wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"爆款美食")) != 0
				)
			{
				spoilsCount++;
			}
		}
	}
	return spoilsCount;
}
// 获取战利品
bool status_3::getTheSpoils() {
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	ROLE_POS rolePos = role::getRolePos();
	int spoilsCount = getTheSpoilsCount();
	if (g_自动模式 == 搬砖)
	{
		if ((rolePos.room.x == 2 && rolePos.room.y == 2 && getCurrentCopyId() == 格蓝迪))
		{
			key.doKeyPress(VK_V);
			if (spoilsCount > 0) {
				Sleep(200);
				key.doKeyPress(VK_X, 150 * spoilsCount);
			}
			return false;
		}
	}
	/*else if (g_自动模式 == 剧情) {
		if (fun::isBossRoom() == true)
		{
			key.doKeyPress(VK_V);
			if (spoilsCount > 0) {
				Sleep(200);
				key.doKeyPress(VK_X, 150 * spoilsCount);
			}
			return false;
		}
	}*/
	
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<DWORD>((mapStartAddress + i * 4));
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (object.code == Code_鸡腿 || object.code == Code_肉块 || object.code == Code_成长之泉水)
			continue;
		if (
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"碎布片")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"最下级硬化剂")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"最下级砥石")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"风化的碎骨")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"钢铁片")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"生锈的铁片")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"破旧的皮革")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"无尽的永恒")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"达人HP药剂")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"达人MP药剂")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"金刚石")) == 0 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"赫仑皇帝的印章")) == 0 ||
			object.name.find(VMProtectDecryptStringW(L"设计图"), 0) != -1 ||
			object.name.find(VMProtectDecryptStringW(L"故事簿"), 0) != -1 ||
			wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"爆款美食")) == 0
			) 
		{

			continue;
		}
		if (object.type == 289 && object.camp == 200)
		{
			if (object.z > 0)
			{
				Sleep(1000);
				return true;
			}
			if (abs(rolePos.x - object.x) > 2 || abs(rolePos.y - object.y) > 2)
			{
				moveRoleToPos(object.x, object.y);
			}
			if (wcscmp(object.name.c_str(), VMProtectDecryptStringW(L"金币")) != 0)
			{
				key.doKeyPress(VK_X);
				Sleep(100);
			}
			return true;
		}
	}
	return false;
}

// 获取通关商店 营火
DWORD status_3::getObjectPointerByCode(int code)
{
	DWORD mapStartAddress = getMapStartAddress();
	DWORD mapObjectCount = getMapObjectCount(mapStartAddress);
	MAP_OBJECT_STRUCT object;
	DWORD objectAddress;
	for (size_t i = 0; i < mapObjectCount; i++) {
		objectAddress = memory.read<int>((mapStartAddress + i * 4));
		if (objectAddress <= 0)continue;
		object = getObjectInfo(objectAddress);
		if (object.code == code)
		{
			return objectAddress;
		}
	}
	return NULL;
}

void status_3::按键_破晓女神()
{
	POS currentRoom = fun::getCurrentRoomPos();
	ROLE_POS rolePos;
	if (getCurrentCopyId() == 格蓝迪)
	{
		if (currentRoom.x == 0 && currentRoom.y == 0)
		{
			Sleep(1000);
			role::releaseSkillByKey(VK_F);
			moveRoleToPos(468, 239);
			role::releaseSkillByKey(VK_G);
		}
		else if (currentRoom.x == 1 && currentRoom.y == 0) {
			Sleep(500);
			key.doKeyPress(VK_Y);
			Sleep(3000);
		}
		else if (currentRoom.x == 2 && currentRoom.y == 0) {
			moveRoleToPos(582, 241);
			role::releaseSkillByKey(VK_A, 300);
		}
		else if (currentRoom.x == 2 && currentRoom.y == 1) {
			moveRoleToPos(550,200);
			key.doKeyPress(VK_NUMPAD1);
			role::releaseSkillByKey(VK_R);
		}
		/*else if (currentRoom.x == 1 && currentRoom.y == 1) {
			moveRoleToPos(653, 272);
			role::releaseSkillByKey(VK_A, 300);
		}
		else if (currentRoom.x == 1 && currentRoom.y == 2) {
			moveRoleToPos(290, 321);
			key.doKeyPress(VK_NUMPAD3);
			role::releaseSkillByKey(VK_A, 300);
		}*/
		else if (currentRoom.x == 2 && currentRoom.y == 2) {
			moveRoleToPos(331, 329);
			key.doKeyPress(VK_NUMPAD3);
			role::releaseSkillByKey(VK_A);
			moveRoleToPos(611, 201);
			role::releaseSkillByKey(VK_T);
			follow(VMProtectDecryptStringW(L"巨人波图拉"));
			role::releaseSkillByKey(VK_Q);
		}
		else if (currentRoom.x == 3 && currentRoom.y == 2) {
			moveRoleToPos(343, 290);
			role::releaseSkillByKey(VK_A, 300);
		}
		else if (currentRoom.x == 3 && currentRoom.y == 1) {
			if (role::getRoleFacing() == 0)
			{
				key.doKeyPress(VK_NUMPAD3);
			}
			key.doKeyPress(VK_W);
			Sleep(3500);
		}
		if (fun::isOpenDoor() == true)
		{

			return;
		}
		follow();
		role::releaseSkillByKey(VK_S);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		else {
			Sleep(1200);
		}
		follow();
		role::releaseSkillByKey(VK_E);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		
		follow();
		role::releaseSkillByKey(VK_G);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		follow();
		role::releaseSkillByKey(VK_H);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		follow();
		role::releaseSkillByKey(VK_D);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		follow();
		role::releaseSkillByKey(VK_S);
		if (fun::isOpenDoor() == true)
		{
			return;
		}
		else {
			Sleep(1200);
		}
		int i = 0;
		while (fun::isOpenDoor() == false && g_自动开关)
		{
			if (fun::passStoryFrame() == false)
			{
				continue;
			}
			follow();
			key.doKeyPress(VK_X, 1000);
			if (i%3 == 0)
			{
				follow();
				role::releaseSkillByKey(VK_S);
			}
			if (i%10 == 0)
			{
				follow();
				role::releaseSkillByKey(VK_E);
			}
			if (i%20 == 0)
			{
				follow();
				role::releaseSkillByKey(VK_G);
			}
			if (i > 50) {
				utils::myprintf(VMProtectDecryptStringA("超时结束自动"));
				key.doKeyPress(VK_ESCAPE);
				key.doKeyPress(VK_ESCAPE);
				g_自动开关 = false;
				return;
			}
			i++;
		}
	}
	else {
		utils::myprintf(VMProtectDecryptStringA("此副本暂不支持"));
	}
}