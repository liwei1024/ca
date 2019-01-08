#pragma once
struct GOODS_INFO
{
	int address;
	int count = 1;
	int level;
	std::wstring name;
	int index;
};
namespace knapsac
{
	DWORD getKnapsacAddress();
	DWORD getKnapsacStartAddress();
	GOODS_INFO getGoodsInfo(int GoodsAddress);
	int getGoodsIndexByGoodsName(std::wstring GoodsName);
	int sendPacketSellThings();
	void outputGoodsInfo();
	POS getGoodsPosByIndex(int index);
	int getGoodsCount();
	void keyPadSellThings();
};

