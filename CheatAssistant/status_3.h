#pragma once
typedef struct _MAP_OBJECT_STRUCT
{
	DWORD address;
	std::wstring name;
	DWORD type;
	DWORD camp;
	DWORD health_point;
	DWORD code;
	int x;
	int y;
	int z;
}MAP_OBJECT_STRUCT,*PMAP_OBJECT_STRUCT;

namespace status_3
{
	void manage();
	void moveToNextRoom();
	DWORD getMapAddress();
	DWORD getMapStartAddress();
	int getMapObjectCount(DWORD map_start_address);
	void moveRoleToPos(int x, int y);
	int getCurrentCopyId();
	std::wstring getCurrentCopyName();
	MAP_OBJECT_STRUCT getObjectInfo(DWORD object_pointer);
	void outputMapObjectInfo();
	int getMonsterCount();
	void fullScreenSkills();
	bool findMonsterZ_AxisMoreThanThe35();
	bool blurryFindMonsterByString(std::wstring string);
	void sortByDistance(std::vector<MAP_OBJECT_STRUCT>& Objects);
	void follow(std::wstring name = L"");
	void 按键_吞噬魔();
	void 按键_蔡依林();
	BOOL getObjectInfoByObjectCode(PMAP_OBJECT_STRUCT object, int objectCode);
	int getTheSpoilsCount();
	bool getTheSpoils();
	DWORD getObjectPointerByCode(int code);
	void 按键_破晓女神();
};
