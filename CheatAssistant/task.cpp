#include "pch.h"
#include "task.h"
#include "role.h"
#include "call.h"
#include "function.h"
#include "send_packet.h"
#include "knapsac.h"

// 不可完成任务
int ignoreTask[] = {
	3191,
	3525,
	3345,
	3413,
	3451,
	3609,
	3521
};

// 遍历所有任务
 void task::traverseAllTaskInfo(DWORD &start_address, size_t &task_count)
{
	start_address = memory.readOffset<DWORD>(__任务基址, { 104 });
	task_count = size_t((memory.readOffset<DWORD>(__任务基址, { 108 }) - start_address) / 4);
	
}
 // 遍历已接任务
 void task::traverseReceivedTaskInfo(DWORD &start_address, size_t &task_count)
{
	start_address = memory.readOffset<DWORD>(__任务基址, { 8 });
	task_count = size_t((memory.readOffset<DWORD>(__任务基址, { 12 }) - start_address) / 12);
}
 // 任务是否已经接受
 bool task::taskIsReceived(int task_id) {
	 DWORD task_start_address = 0;
	 size_t task_count = 0;
	 traverseReceivedTaskInfo(task_start_address, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 if (memory.read<int>(memory.read<DWORD>(task_start_address + i * 12)) == task_id)
		 {
			 return true;
		 }
	 }
	 return false;
 }
 // 获取使命任务副本Id
 int task::getMissionTaskCopyId() {
	 DWORD task_start_address = 0;
	 DWORD address;
	 TASK_STRUCT task;
	 size_t task_count = 0;
	 int copyId = 1;
	 traverseAllTaskInfo(task_start_address, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 address = memory.read<DWORD>(task_start_address + i * 4);
		 if (address <= 0)
		 {
			 continue;
		 }
		 task = traverseTaskObject(address);
		 if (task.type == 26)
		 {
			 copyId = task.copy_id;
		 }
	 }
	 return copyId;
 }
 // 获取使命任务Id
 int task::getMissionTaskId() {
	 DWORD task_start_address = 0;
	 DWORD address;
	 TASK_STRUCT task;
	 size_t task_count = 0;
	 int taskId = 1;
	 traverseAllTaskInfo(task_start_address, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 address = memory.read<DWORD>(task_start_address + i * 4);
		 if (address <= 0)
		 {
			 continue;
		 }
		 task = traverseTaskObject(task.task_id);
		 if (task.type == 26)
		 {
			 taskId = task.copy_id;
		 }
	 }
	 return taskId;
 }
// 任务是否有主线任务
 bool task::isThearMainTask() {
	 DWORD task_start_address = 0;
	 size_t task_count = 0;
	 DWORD address = 0;
	 int task_id = 0;
	 traverseAllTaskInfo(task_start_address, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 address = memory.read<DWORD>(task_start_address + i * 4);
		 if (address == 0)
		 {
			 continue;
		 }
		 task_id = memory.read<int>(address);
		 /*if (utils::hasIntArray(task_id, ignoreTask, sizeof(ignoreTask) / sizeof(int)) != -1)
		 {
			 continue;
		 }*/
		 if (memory.read<int>(address + 308) == 0)
		 {
			 return true;
		 }
	 }
	 return false;
 }
 // 任务是否可以忽略（跳过） 
 bool task::currentMainTaskIsCanIgnore() {
	 DWORD task_start_address = 0;
	 size_t task_count = 0;
	 DWORD address;
	 traverseReceivedTaskInfo(task_start_address, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 address = memory.read<DWORD>(task_start_address + i * 12);
		 if (memory.read<int>(address + 308) == 0)
		 {
			 if (memory.read<int>(address + 452) < role::getRoleLevel())
			 {
				 return true;
			 }
			 break;
		 }
	 }
	 return false;
 }
 //获取任务完成次数
 int task::getAaskDegree(int task_id) {
	 DWORD task_start_address = 0;
	 size_t task_count = 0;
	 TASK_STRUCT task;
	 DWORD address;
	 int taskDegree = 0;
	 int tempArray[] = {0,0,0};
	 traverseReceivedTaskInfo(task_start_address, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 address = memory.read<DWORD>(task_start_address + i * 12);
		 if (address <= 0)
		 {
			 continue;
		 }
		 if (memory.read<int>(memory.read<DWORD>(task_start_address + i * 12)) == task_id)
		 {
			 taskDegree = fun::decrypt((int)(task_start_address + i * 12 + 4));
			 tempArray[0] = (int)ceil(taskDegree % 512);
			 tempArray[1] = (int)ceil(taskDegree / 512);
			 tempArray[2] = (int)ceil(taskDegree / 262144);
			/* printf("%d %d %d\n", tempArray[0], tempArray[1], tempArray[2]);
			 std::sort(tempArray, tempArray + 3, cmp1);
			 printf("%d %d %d\n", tempArray[0], tempArray[1], tempArray[2]);*/
			 std::sort(tempArray, tempArray + 3);
			 //printf("%d %d %d\n", tempArray[0], tempArray[1], tempArray[2]);
			 taskDegree = tempArray[2];
		 }
	 }
	 return taskDegree;
 }
 //遍历任务信息
 TASK_STRUCT task::traverseTaskObject(DWORD ObjectPointer)
{
	TASK_STRUCT task;
	if (ObjectPointer != 0)
	{
		task.address = ObjectPointer;
		task.task_id = memory.read<int>(ObjectPointer);
		task.type = memory.read<DWORD>(ObjectPointer + 308);
		task.copy_id = memory.read<int>(memory.read<DWORD>(ObjectPointer + 696));
		task.materials = memory.read<int>(ObjectPointer + 1428); //材料
		task.task_length = memory.read<int>(ObjectPointer + 28);
		task.condition_type = memory.readWString(memory.read<DWORD>(ObjectPointer + 720), 100);
		if (task.task_length > 7)
		{
			task.name = memory.readWString(memory.read<DWORD>(ObjectPointer + 8), 100);
		}
		else {

			task.name = memory.readWString(ObjectPointer + 8, 100);
		}
		task.done_level = memory.read<int>(ObjectPointer + 452);
		
		if (task.type == 0)
		{
			task.is_received = taskIsReceived(task.task_id);
			task.taskDegree = getAaskDegree(task.task_id);
			task.isCanIgnore = (task.done_level < role::getRoleLevel());
		}
		
	}

	return task;
}
 // 打印任务信息
 void task::outputTaskInfo(int type) 
 {
	 DWORD task_start_address = 0;
	 size_t task_count = 0;
	 DWORD address;
	 TASK_STRUCT task;

	 if (type == 0 || type == 2)
	 {
		 traverseAllTaskInfo(task_start_address, task_count);
	 }
	 else if(type == 1){
		 traverseReceivedTaskInfo(task_start_address, task_count);
	 }
	 utils::myprintf(VMProtectDecryptStringA("task_start_address->:%x"), RED, task_start_address);
	 utils::myprintf(VMProtectDecryptStringA("task_count->:%zd"),RED, task_count);
	 for (size_t i = 0; i < task_count; i++)
	 {
		 address = memory.read<DWORD>(task_start_address + i * (type==1?12:4));

		 if (address <= 0)
		 {
			 continue;
		 }
		 task = traverseTaskObject(address);
		 if (type == 2 && task.type != 0)
		 {
			 continue;
		 }
		 utils::myprintf(VMProtectDecryptStringA("==================================================="));
		 //utils::myprintf(VMProtectDecryptStringA("编号->:%d"), GREEN, i);
		 utils::myprintf(VMProtectDecryptStringA("address->:%x"), GREEN, task.address);
		 utils::mywprintf(VMProtectDecryptStringW(L"name->:%ws"), GREEN, task.name.c_str());
		 utils::myprintf(VMProtectDecryptStringA("type->:%d"), GREEN, task.type);
		 utils::myprintf(VMProtectDecryptStringA("copy_id->:%d"), GREEN, task.copy_id);
		 utils::myprintf(VMProtectDecryptStringA("task_id->:%d"), GREEN, task.task_id);
		 utils::myprintf(VMProtectDecryptStringA("materials->:%d"), GREEN, task.materials);
		 utils::myprintf(VMProtectDecryptStringA("task_length->:%d"), GREEN, task.task_length);
		 utils::mywprintf(VMProtectDecryptStringW(L"condition_type->:%ws"), GREEN, task.condition_type.c_str());
		 utils::myprintf(VMProtectDecryptStringA("is_received->:%d"), GREEN, task.is_received);
		 utils::myprintf(VMProtectDecryptStringA("done_level->:%d"), GREEN, task.done_level);
		 utils::myprintf(VMProtectDecryptStringA("taskDegree->:%d"), GREEN, task.taskDegree);
	 }
 }
 // 自动主线任务
 void task::autoMasterTask()
{
	DWORD task_start_address;
	DWORD address;
	size_t task_count;
	TASK_STRUCT task;
	if (isThearMainTask() == false)
	{
		printf(VMProtectDecryptStringA("未获取到主线任务,自动选择角色适应的副本升级\n"));
		// 这里应该去弄个自适应地图的
		fun::chooseTheAppropriateMap();
		return;
	}
	traverseAllTaskInfo(task_start_address, task_count);
	for (size_t i = 0; i < task_count; i++)
	{
		address = memory.read<DWORD>(task_start_address + i * 4);
		if (address <= 0)
		{
			continue;
		}
		task = traverseTaskObject(address);

		if (task.type == 0)
		{
			if (task.is_received == false)
			{
				call::接受Call(task.task_id);
				utils::mywprintf(VMProtectDecryptStringW(L"接受任务 %s 任务ID %d 任务副本ID %d"), PINK, task.name.c_str(), task.task_id, task.copy_id);
				Sleep(1000);
				return;
			}
			if (task.isCanIgnore) {
				printf(VMProtectDecryptStringA("跳过任务\n"));
				call::跳过Call();
				Sleep(1000);
				return;
			}
			//printf(VMProtectDecryptStringA("hasIntArray->%d\n"), utils::hasIntArray(task.task_id, ignoreTask, sizeof(ignoreTask) / sizeof(int)));

			if (utils::hasIntArray(task.task_id, ignoreTask,sizeof(ignoreTask)/sizeof(int)) > -1)
			{
				//printf(VMProtectDecryptStringA("咩咩咩咩咩咩咩咩咩咩咩咩 %d\n"), task.task_id);
				fun::chooseTheAppropriateMap(task.task_id);
				return;
			}
			if (task.copy_id > 0 && task.taskDegree > 0)
			{
				utils::mywprintf(VMProtectDecryptStringW(L"开始任务 %s 任务ID %d 任务副本ID %d"), PINK, task.name.c_str(), task.task_id, task.copy_id);
				CITY_INFO city_info;
				ROLE_POS rolePos = role::getRolePos();
				call::区域Call(&city_info, task.copy_id, task.task_id);
				if (
					rolePos.room.x != city_info.room.x ||
					rolePos.room.y != city_info.room.y
					)
				{
					SendPacket().城镇瞬移(city_info, knapsac::getGoodsIndexByGoodsName(VMProtectDecryptStringW(L"瞬间移动药剂")));
					Sleep(1000);
				}
				SendPacket().进入选图();
				while (fun::getGameStatus() != 2)
				{
					Sleep(1000);
				}
				SendPacket().选择副本(task.copy_id, 0, 剧情, task.task_id);
				while (fun::getGameStatus() != 3)
				{
					Sleep(1000);
				}
				return;
			}
			else if (task.materials > 0) {
				printf(VMProtectDecryptStringA("材料任务,选择自适应副本\n"));
				//g_自动开关 = false;
				fun::chooseTheAppropriateMap(task.task_id);
				return;
			}
			else {
				if (taskIsReceived(task.task_id) == false)
				{
					call::接受Call(task.task_id);
					utils::mywprintf(VMProtectDecryptStringW(L"接受任务 %s 任务ID %d 任务副本ID %d"), PINK, task.name.c_str(), task.task_id, task.copy_id);
					Sleep(1000);
					return;
				}
				else {
					utils::mywprintf(VMProtectDecryptStringW(L"完成任务 %s"), PINK, task.name.c_str());
					for (int i = 0; i < task.taskDegree; i++)
					{
						call::完成Call(task.task_id);
						Sleep(200);
					}
					key.doKeyPress(VK_ESCAPE);
					call::提交Call(task.task_id);
					Sleep(1000);
				}
				return;
			}
		}
		else if (task.type == 26) {

		}
		
	}
}
