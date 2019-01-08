#pragma once
struct TASK_STRUCT
{
	DWORD address = 0;
	std::wstring name;
	DWORD type = 0;
	int copy_id = 0;
	int task_id = 0;
	int materials = 0;
	int task_length = 0;
	std::wstring condition_type;
	bool is_received = false; //任务是否已经接受
	int done_level = 0;//任务可完成等级
	int taskDegree = 0;
	bool isCanIgnore = false;
};
namespace task
{
	void traverseAllTaskInfo(DWORD & start_address, size_t & task_count);
	void traverseReceivedTaskInfo(DWORD & start_address, size_t & task_count);
	bool taskIsReceived(int task_id);
	int getMissionTaskCopyId();
	int getMissionTaskId();
	bool isThearMainTask();
	bool currentMainTaskIsCanIgnore();
	int getAaskDegree(int task_id);
	TASK_STRUCT traverseTaskObject(DWORD ObjectPointer);
	void outputTaskInfo(int type);
	void autoMasterTask();
};

