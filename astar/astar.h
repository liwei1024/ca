#ifndef __ASTAR_H__
#define __ASTAR_H__

struct AStarPos
{
	int x, y, z = 0;
};

struct AStarMapInfo
{
	int width;
	int height;
	std::vector<DWORD> room_channel;
	AStarPos start_room;
	AStarPos end_room;
};

struct AStarRoomInfo
{
	AStarPos coordinate;
	AStarPos parent_coordinate;
	int weight;
	int numbering;
	bool up;
	bool bottom;
	bool left;
	bool right;
};
namespace astar
{
	BOOL has_list(std::vector<AStarRoomInfo> list, AStarRoomInfo to_search_room);
	int find_min_weight_key(std::vector<std::vector<AStarRoomInfo>> room_array, std::vector<AStarRoomInfo> open_list);
	//VOID read_map_info(AStarMapInfo & map_info, int type);
	VOID create_room_array(AStarMapInfo map_info, std::vector<std::vector<AStarRoomInfo>>& room_array);
	VOID search_path(AStarMapInfo map_info, std::vector<std::vector<AStarRoomInfo>>& room_array, AStarRoomInfo & boss_room);
	VOID recall_path(AStarMapInfo map_info, std::vector<std::vector<AStarRoomInfo>> room_array, AStarRoomInfo room_info, std::vector<int>& path);
	int GetDirection(AStarMapInfo map_info);
};
#endif // !__ASTAR_H__




