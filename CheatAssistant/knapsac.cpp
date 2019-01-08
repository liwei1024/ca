#include "pch.h"
#include "knapsac.h"
#include "function.h"


DWORD knapsac::getKnapsacAddress()
{
	return memory.read<int>(__背包基址);
}

DWORD knapsac::getKnapsacStartAddress()
{
	return memory.read<int>(getKnapsacAddress() + 88) + 36;
}

 GOODS_INFO knapsac::getGoodsInfo(int GoodsAddress)
{
	GOODS_INFO _GoodsInfo;
	_GoodsInfo.address = GoodsAddress;
	_GoodsInfo.name = memory.readWString(memory.read<int>(GoodsAddress + 36), 52);
	_GoodsInfo.level = memory.read<int>(GoodsAddress + 376);
	return _GoodsInfo;
}

 int knapsac::getGoodsIndexByGoodsName(std::wstring GoodsName)
{
	DWORD StartAddress = getKnapsacStartAddress();
	GOODS_INFO _GoodsInfo;
	DWORD GoodsAddress;
	for (size_t i = 0; i < 279; i++)
	{
		GoodsAddress = memory.read<int>(ULONG(StartAddress + i * 4));
		if (GoodsAddress == 0 || GoodsAddress == NULL)continue;
		_GoodsInfo = getGoodsInfo(GoodsAddress);
		_GoodsInfo.index = int(i + 9);
		//output_bebug_wstring(L"%ws --- %ws", _GoodsInfo.name.c_str(), GoodsName.c_str());
		//if (_GoodsInfo.name.c_str() == GoodsName.c_str())equals
		if (wcscmp(_GoodsInfo.name.c_str(), GoodsName.c_str()) == 0)
		{
			return _GoodsInfo.index;
		}
	}
	return -1;
}

 int knapsac::sendPacketSellThings()
{
	DWORD StartAddress = getKnapsacStartAddress();
	GOODS_INFO _GoodsInfo;
	DWORD GoodsAddress;
	for (size_t i = 0; i < 55; i++)
	{
		GoodsAddress = memory.read<int>(ULONG(StartAddress + i * 4));
		if (GoodsAddress == 0 || GoodsAddress == NULL)continue;
		_GoodsInfo = getGoodsInfo(GoodsAddress);
		_GoodsInfo.index = int(i) + 9;
		if (
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"传承"), 0) == -1) &&
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"周年"), 0) == -1) &&
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"活动"), 0) == -1)
			)
		{
			if (
				_GoodsInfo.level == 0 ||
				_GoodsInfo.level == 1 ||
				_GoodsInfo.level == 2
				)
			{
				//main_thread_exec_call(Send_组包卖物, { _GoodsInfo.index });
				Sleep(500);
			}
		}
	}
	return -1;
}

 void knapsac::outputGoodsInfo()
{
	DWORD StartAddress = getKnapsacStartAddress();
	GOODS_INFO _GoodsInfo;
	DWORD GoodsAddress;
	for (size_t i = 0; i < 279; i++)
	{
		GoodsAddress = memory.read<int>(ULONG(StartAddress + i * 4));
		if (!GoodsAddress)
		{
			continue;
		}
		_GoodsInfo = getGoodsInfo(GoodsAddress);
		_GoodsInfo.index = int(i) + 9;
		utils::mywprintf(VMProtectDecryptStringW(L"====================================="));
		utils::mywprintf(VMProtectDecryptStringW(L"地址 0x%x"),YELLOW, _GoodsInfo.address);
		utils::mywprintf(VMProtectDecryptStringW(L"等级 %d"), YELLOW, _GoodsInfo.level);
		utils::mywprintf(VMProtectDecryptStringW(L"名称 %s"), YELLOW, _GoodsInfo.name.c_str());
		utils::mywprintf(VMProtectDecryptStringW(L"下标 %d"), YELLOW, _GoodsInfo.index);
	}
}

 POS knapsac::getGoodsPosByIndex(int index)
{
	POS pos;
	int x, y;
	if (index >= 0 and index <= 7)
	{
		x = index * 30;
		y = 0;
	}
	else if (index >= 8 and index <= 15)
	{
		x = (index - 8) * 30;
		y = 30;
	}
	else if (index >= 16 and index <= 23)
	{
		x = (index - 16) * 30;
		y = 60;
	}
	else if (index >= 24 and index <= 31)
	{
		x = (index - 24) * 30;
		y = 90;
	}
	else if (index >= 32 and index <= 39)
	{
		x = (index - 32) * 30;
		y = 120;
	}
	else if (index >= 40 and index <= 47)
	{
		x = (index - 40) * 30;
		y = 150;
	}
	else if (index >= 48 and index <= 55)
	{
		x = (index - 48) * 30;
		y = 180;
	}
	pos.x = gameWindowInfo.left + 623 + x;
	pos.y = gameWindowInfo.top + 301 + y;
	return pos;
}

 int knapsac::getGoodsCount()
{
	DWORD StartAddress = getKnapsacStartAddress();
	GOODS_INFO _GoodsInfo;
	DWORD GoodsAddress;
	int count = 0;
	for (size_t i = 0; i < 55; i++)
	{
		GoodsAddress = memory.read<int>(ULONG(StartAddress + i * 4));
		if (GoodsAddress == 0 || GoodsAddress == NULL)continue;
		_GoodsInfo = getGoodsInfo(GoodsAddress);
		_GoodsInfo.index = int(i) + 9;
		if (
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"传承"), 0) == -1) &&
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"周年"), 0) == -1) &&
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"活动"), 0) == -1)
			)
		{
			if (
				_GoodsInfo.level == 0 ||
				_GoodsInfo.level == 1 ||
				_GoodsInfo.level == 2
				)
			{
				count++;
			}
		}
	}
	return count;
}

 void knapsac::keyPadSellThings()
{
	if (getGoodsCount() < 13) {
		return;
	}
	DWORD StartAddress = getKnapsacStartAddress();
	GOODS_INFO _GoodsInfo;
	DWORD GoodsAddress;
	POS goods_pos;
	int failureNumber = 0;//操作失败计次
	for (size_t i = 0; i < 55; i++)
	{
		if (fun::passStoryFrame() == false) {
			utils::myprintf(VMProtectDecryptStringA("执行卖物时，有对话款存在，关闭本次卖物"));
			return;
		}
		if (g_自动开关 == false)
		{
			utils::myprintf(VMProtectDecryptStringA("执行卖物时，自动模式被停止，关闭本次卖物"));
			return;
		}
		if (fun::isBossRoom() == false) {
			utils::myprintf(VMProtectDecryptStringA("卖物失败，执行卖物时不在boss房间"));
			return;
		}
		if (failureNumber > 3)
		{
			key.doKeyPress(VK_ESCAPE);
			key.doKeyPress(VK_SPACE);
			utils::myprintf(VMProtectDecryptStringA("卖物失败，失败次数超过限定次数 关闭本次卖物"));
			return;
		}
		
		GoodsAddress = memory.read<int>(ULONG(StartAddress + i * 4));
		if (GoodsAddress == 0 || GoodsAddress == NULL)continue;
		_GoodsInfo = getGoodsInfo(GoodsAddress);
		if (
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"传承"), 0) == -1) &&
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"周年"), 0) == -1) &&
			(_GoodsInfo.name.find(VMProtectDecryptStringW(L"活动"), 0) == -1)
			)
		{
			if (
				_GoodsInfo.level == 0 ||
				_GoodsInfo.level == 1 ||
				_GoodsInfo.level == 2
				)
			{
				if (memory.read<int>(__鼠标状态) != 5)
				{
					key.setMousePos(gameWindowInfo.left + 192, gameWindowInfo.top + 518);
					Sleep(1000);
					key.mouseClick();
					Sleep(1000);
					if (memory.read<int>(__鼠标状态) == 5) {
						key.setMousePos(gameWindowInfo.left + 630, gameWindowInfo.top + 278);
						Sleep(100);
						key.mouseClick();
						Sleep(100);
					}
					else {
						failureNumber++;
						continue;
					}
				}
				goods_pos = getGoodsPosByIndex(int(i));
				key.setMousePos(goods_pos.x, goods_pos.y);
				Sleep(100);
				key.mouseDoubleClick(100);
			}
		}
	}
}
