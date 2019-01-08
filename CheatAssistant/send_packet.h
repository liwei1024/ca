#pragma once
class SendPacket
{
public:
	void 返回角色();
	void 选择角色(int role_index);
	void 城镇瞬移(CITY_INFO cityInfo, int goods_index = 0);
	void 进入选图();
	void 选择副本(int 副本编号, int 副本难度, int 副本模式 = 搬砖, int 任务ID = 0);
	void 组包拾取(int loot_address, int x, int y);
	void 组包翻牌();
	void 组包卖物(int 物品下标);
	//void 组包回城();
private:
	std::vector<byte> v_shell_code;
	void 缓冲Call(int cdov);
	void 密包Call(int wtud, int taya);
	void 发包Call(bool is_send = true);
};

