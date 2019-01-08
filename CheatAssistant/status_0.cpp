#include "pch.h"
#include "status_0.h"
#include "function.h"

void status_0::manage()
{
	if ((getRoleCount() - 1) == getRoleIndex()) {
		utils::mywprintf(VMProtectDecryptStringW(L"所有角色刷图完毕，自动关闭"));
		g_自动开关 = false;
	}
	else {
		selectRole(-1);
	}

}

int status_0::getRoleIndex()
{
	int role_index = memory.readOffset<int>(__遍历取值, { __角色下标偏移 });
	return role_index;

}

int status_0::getRoleCount()
{
	int role_count = memory.readOffset<int>(__遍历取值, { __当前角色数量,__角色数量偏移2 });
	return role_count;
}

int status_0::getRoleMaxCount()
{
	int role_max_count = memory.readOffset<int>(__遍历取值, { __最大角色数量,__角色数量偏移2 });
	return role_max_count;
}

void status_0::selectRole(int roleIndex) 
{
	if (roleIndex == -1)
	{
		roleIndex = getRoleIndex() + 1;
	}
	while (fun::getGameStatus() == 0 && g_自动开关)
	{
		if (roleIndex != getRoleIndex())
		{
			key.doKeyPress(VK_RIGHT);
		}
		//doKeyPress(VK_RETURN);
		key.setMousePos(gameWindowInfo.left + 534, gameWindowInfo.top + 550);
		key.mouseClick();
		Sleep(3000);
	}
}