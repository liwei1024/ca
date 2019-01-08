#include <vector>
#include <windows.h>
#include "astar.h"

//#ifndef __CONSTS_H__
//#include "../consts.h"
//#endif // !__CONSTS_H__
//
//#ifndef __MEMORY_H__
//#include "../memory/memory.h"
//#endif
//
//using namespace memory;

unsigned char orientation_vector[16][4] = {
	// 左 右 上 下 
	{ 0, 0, 0, 0 },//0	无
	{ 0, 1, 0, 0 },//1	右
	{ 0, 0, 1, 0 },//2	上
	{ 0, 1, 1, 0 },//3	右	上
	{ 1, 0, 0, 0 },//4	左
	{ 1, 1, 0, 0 },//5	左	右
	{ 1, 0, 1, 0 },//6	左	上
	{ 1, 1, 1, 0 },//7	左	右	上
	{ 0, 0, 0, 1 },//8	下	
	{ 0, 1, 0, 1 },//9	右	下
	{ 0, 0, 1, 1 },//10	上	下
	{ 0, 1, 1, 1 },//11	右	上	下
	{ 1, 0, 0, 1 },//12	左	下
	{ 1, 1, 0, 1 },//13	左	右	下
	{ 1, 0, 1, 1 },//14	左	上	下
	{ 1, 1, 1, 1 } //15	左	右	上	下
};
BOOL astar::has_list(std::vector<AStarRoomInfo> list, AStarRoomInfo to_search_room)
{
	for (unsigned i = 0; i < list.size(); i++)
	{
		if (list[i].coordinate.x == to_search_room.coordinate.x && list[i].coordinate.y == to_search_room.coordinate.y)
		{
			return true;
		}
	}
	return false;
}
int astar::find_min_weight_key(std::vector<std::vector<AStarRoomInfo>> room_array, std::vector<AStarRoomInfo> open_list)
{
	int key = 0;
	int min_weight;
	int weight;
	for (unsigned i = 0; i < open_list.size(); i++)
	{
		weight = room_array[open_list[i].coordinate.y][open_list[i].coordinate.x].weight;
		if (i == 0 || weight < min_weight) {
			min_weight = weight;
			key = i;
		}
	}
	return key;
}
//VOID astar::read_map_info(AStarMapInfo &map_info, int type)
//{
//	/*int 临时地址一;
//	int 临时地址二;
//	int 格子数量;
//	int 偏移地址一;
//	int 偏移地址二;
//	int 宽高地址;
//	int 数组地址;
//	偏移地址一 = read<int>(read<int>(__商店基址 - 8) + __时间基址);
//	偏移地址二 = read<int>(偏移地址一 + __门型偏移);
//	宽高地址 = read<int>(偏移地址二 + __宽高偏移);
//	数组地址 = read<int>(偏移地址二 + __数组偏移);
//	临时地址二 = decrypt(偏移地址二 + __BOSS房间X - 24) + 1;
//	临时地址一 = read<int>(数组地址 + 临时地址二 * 20 - 16);
//	map_info.width = read<int>(宽高地址 + 临时地址二 * 8 - 8);
//	map_info.height = read<int>(宽高地址 + 临时地址二 * 8 - 4);
//	格子数量 = map_info.width * map_info.height;
//	for (int i = 0; i < 格子数量; i++)
//	{
//		map_info.room_channel.insert(map_info.room_channel.end(), read<int>(临时地址一 + i * 4));
//	}
//	map_info.start_room.x = read<int>(偏移地址一 + __A星_当前房间X);
//	map_info.start_room.y = read<int>(偏移地址一 + __A星_当前房间Y);
//	if (type == 1) {
//		map_info.end_room.x = decrypt(偏移地址二 + __BOSS房间X);
//		map_info.end_room.y = decrypt(偏移地址二 + __BOSS房间Y);
//	}
//	else if (type == 2) {
//		map_info.end_room.x = decrypt(偏移地址二 + __深渊房间X);
//		map_info.end_room.y = decrypt(偏移地址二 + __深渊房间Y);
//	}*/
//	////map_info = 地图遍历(模式);
//	//map_info.width = 4;
//	//map_info.height = 3;
//	////map_info.name = "ge lan di";
//	//map_info.room_channel = { 1,5,12,0,0,9,14,8,0,3,7,6 };
//	//map_info.end_room.x = 0;
//	//map_info.end_room.y = 0;
//	//map_info.end_room.x = 3;
//	//map_info.end_room.y = 1;
//}
VOID astar::create_room_array(AStarMapInfo map_info, std::vector<std::vector<AStarRoomInfo>>& room_array)
{
	AStarRoomInfo temp_room_info;
	// 1.初始化容器大小
	room_array.clear();
	room_array.resize(map_info.height);
	for (int x = 0; x < map_info.height; x++)
	{
		room_array[x].resize(map_info.width);
	}
	// 2.生成
	int numbering = 0;
	for (int y = 0; y < map_info.height; y++)
	{
		for (int x = 0; x < map_info.width; x++) {
			temp_room_info.coordinate.x = x;
			temp_room_info.coordinate.y = y;
			temp_room_info.up = orientation_vector[map_info.room_channel[numbering]][2] == 1 ? true : false;
			temp_room_info.bottom = orientation_vector[map_info.room_channel[numbering]][3] == 1 ? true : false;
			temp_room_info.left = orientation_vector[map_info.room_channel[numbering]][0] == 1 ? true : false;
			temp_room_info.right = orientation_vector[map_info.room_channel[numbering]][1] == 1 ? true : false;
			temp_room_info.weight = abs(map_info.end_room.x - x) + abs(map_info.end_room.y - y);
			temp_room_info.numbering = numbering;
			room_array[y][x] = temp_room_info;
			numbering++;
		}
	}
}
VOID astar::search_path(AStarMapInfo map_info, std::vector<std::vector<AStarRoomInfo>>& room_array, AStarRoomInfo & boss_room)
{
	std::vector<AStarRoomInfo> open_list;
	std::vector<AStarRoomInfo> close_list;
	std::vector<AStarRoomInfo>::iterator iter;
	AStarRoomInfo current_room;
	AStarRoomInfo to_search_room;
	int min_weight_key, min_weight;
	open_list.insert(open_list.end(), room_array[map_info.start_room.y][map_info.start_room.x]);
	while (open_list.size() > 0)
	{
		min_weight_key = find_min_weight_key(room_array, open_list);
		current_room = open_list[min_weight_key];
		min_weight = current_room.weight;

		if (min_weight == 0)
		{
			boss_room = current_room;
			boss_room.parent_coordinate = boss_room.coordinate;
			return;
		}
		if (current_room.up == TRUE)
		{
			to_search_room = room_array[current_room.coordinate.y - 1][current_room.coordinate.x];
			if (has_list(open_list, to_search_room) == false && has_list(close_list, to_search_room) == false)
			{
				open_list.insert(open_list.end(), to_search_room);
				room_array[to_search_room.coordinate.y][to_search_room.coordinate.x].parent_coordinate = current_room.coordinate;
			}
		}
		if (current_room.bottom == TRUE)
		{
			to_search_room = room_array[current_room.coordinate.y + 1][current_room.coordinate.x];
			if (has_list(open_list, to_search_room) == false && has_list(close_list, to_search_room) == false)
			{
				open_list.insert(open_list.end(), to_search_room);
				room_array[to_search_room.coordinate.y][to_search_room.coordinate.x].parent_coordinate = current_room.coordinate;
			}
		}
		if (current_room.left == TRUE)
		{
			to_search_room = room_array[current_room.coordinate.y][current_room.coordinate.x - 1];
			if (has_list(open_list, to_search_room) == false && has_list(close_list, to_search_room) == false)
			{
				open_list.insert(open_list.end(), to_search_room);
				room_array[to_search_room.coordinate.y][to_search_room.coordinate.x].parent_coordinate = current_room.coordinate;
			}
		}
		if (current_room.right == TRUE)
		{
			to_search_room = room_array[current_room.coordinate.y][current_room.coordinate.x + 1];
			if (has_list(open_list, to_search_room) == false && has_list(close_list, to_search_room) == false)
			{
				open_list.insert(open_list.end(), to_search_room);
				room_array[to_search_room.coordinate.y][to_search_room.coordinate.x].parent_coordinate = current_room.coordinate;
			}
		}
		iter = open_list.begin() + min_weight_key;
		open_list.erase(iter);
		close_list.insert(close_list.end(), current_room);
	}
}
VOID astar::recall_path(AStarMapInfo map_info, std::vector<std::vector<AStarRoomInfo>> room_array, AStarRoomInfo room_info, std::vector<int>& path)
{
	if (room_info.coordinate.x < room_info.parent_coordinate.x)
	{
		path.insert(path.end(), 0);
	}
	else if (room_info.coordinate.x > room_info.parent_coordinate.x) {
		path.insert(path.end(), 1);
	}
	else if (room_info.coordinate.y < room_info.parent_coordinate.y) {
		path.insert(path.end(), 2);
	}
	else if (room_info.coordinate.y > room_info.parent_coordinate.y) {
		path.insert(path.end(), 3);
	}
	if (room_info.parent_coordinate.x != map_info.start_room.x || room_info.parent_coordinate.y != map_info.start_room.y) {
		recall_path(map_info, room_array, room_array[room_info.parent_coordinate.y][room_info.parent_coordinate.x], path);
	}
}
int astar::GetDirection(AStarMapInfo map_info)
{
	printf(" map_info.width->:%d map_info.height->:%d\n", map_info.width, map_info.height);
	//AStarMapInfo map_info;
	std::vector<std::vector<AStarRoomInfo>> room_array;
	AStarRoomInfo boss_room;
	std::vector<int> path;
	// 1.读取地图信息
	//read_map_info(map_info, type);
	// 2.创建房间数组
	create_room_array(map_info, room_array);
	// 3.搜索路径
	search_path(map_info, room_array, boss_room);
	// 4.回溯路径
	recall_path(map_info, room_array, boss_room, path);
	/*bulletin(L"size %zd\n", path.size());
	for (SIZE_T i = 0; i < path.size(); i++)
	{
		if (path[i] == 0)
		{
			bulletin(L"左\n");
		}
		else if (path[i] == 1) {
			bulletin(L"右\n");
		}
		else if (path[i] == 2) {
			bulletin(L"上\n");
		}
		else if (path[i] == 3) {
			bulletin(L"下\n");
		}
	}*/
	return path[path.size() - 1];
}