#pragma once
#define 城镇 0
#define 打怪 1
#define 捡物 2
#define 过图 3
namespace role {
	int getRoleLevel();
	std::wstring getRoleJobName();
	int getCurrentRoleFatigueValue();
	ROLE_POS getRolePos();
	int getRoleStatus();
	int getRoleFacing();
	void releaseSkillByKey(int keyCode, int s = 100);
	void moveRoleToPos_城镇(ROLE_POS targetPos);
	void moveRoleToPos_过图(ROLE_POS targetPos);
	void moveRoleToPos_捡物(ROLE_POS targetPos);
	void moveRoleToPos_打怪(ROLE_POS targetPos);
	std::vector<int> moveRoleToPos_计算方向(ROLE_POS currentPos, ROLE_POS targetPos);
	void moveRoleToPos_卡点处理(ROLE_POS targetPos, std::map<const char*, bool>& cardPointList);
	void moveRoleToPos(ROLE_POS targetPos,int type = 城镇);
	int getRoleMoveSpeed();
	void moveRoleToPos2(ROLE_POS targetPos, int type);
	void ignoreBuild(bool enbale);
	void ignoreTerrain(bool enbale);
}

