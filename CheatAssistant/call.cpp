#include "pch.h"
#include "call.h"
#include "function.h"
#include "role.h"
#include "status_3.h"
/*
__asm
{
	jn_call - 60                    - pushad 
	7FFC05E1- 9C                    - pushfd 
	7FFC05E2- B9 E001FC7F           - mov ecx,jn_struct { [600E0000] }
	7FFC05E7- B8 D0BB5002           - mov eax,dnf.exe+210BBD0 { [83EC8B55] }
	7FFC05EC- FF D0                 - call eax
	7FFC05EE- 9D                    - popfd 
	7FFC05EF- 61                    - popad 
	7FFC05F0- C3                    - ret 

}
*/
void call::技能Call(int pointer, int code, int damage, int x, int y, int z)
{
	int skill_struct[26] = {};
	skill_struct[0] = pointer;//触发指针;
	skill_struct[2] = code; //代码
	skill_struct[3] = damage; //伤害
	skill_struct[6] = x;//x
	skill_struct[7] = y;//y
	skill_struct[8] = z;//z
	skill_struct[22] = 0; //大小
	skill_struct[23] = 0;
	skill_struct[24] = 65535;
	skill_struct[25] = 65535;

	byte opcodes[] = {
		0x60,
		0x9c,
		0xb9,0x00,0x00,0x00,0x00,
		0xb8,0x00,0x00,0x00,0x00,
		0xff,0xd0,
		0x9d,
		0x61,
		0xc3
	};

	*(int*)(opcodes + 3) = __CALL参数;
	*(int*)(opcodes + 8) = __技能CALL;

	fun::remoteMainThreadCall(opcodes, sizeof(opcodes), skill_struct, sizeof(skill_struct));

}

/*
__asm
{
	push z
	push y
	push x
	push 伤害
	push 代码
	push 触发指针
	mov edi, __释放CALL
	mov eax, edi
	call eax
	add esp, 24
}
*/
void call::释放Call(int pointer, int code, int damage, int x, int y, int z)
{
	byte opcodes[] = {
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0xBF,0x00,0x00,0x00,0x00,
		0x8B,0xC7,
		0xFF,0xD0,
		0x83,0xC4,0x24,
		0xC3,
	};
	*(int*)(opcodes + 1) = z;
	*(int*)(opcodes + 6) = y;
	*(int*)(opcodes + 11) = x;
	*(int*)(opcodes + 16) = damage;
	*(int*)(opcodes + 21) = code;
	*(int*)(opcodes + 26) = pointer;
	*(int*)(opcodes + 31) = __释放CALL;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}
/*
__asm
{
	mov esi, object_pointer
	mov edi, dword ptr[esi]
	push z
	push y
	push x
	mov eax, dword ptr[edi + 0xb4]
	mov ecx, esi
	call eax
}
*/
void call::坐标Call(int pointer, int x, int y, int z)
{
	byte opcodes[] = {
		0xBE,0x00,0x00,0x00,0x00,
		0x8B,0x3E,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x8B,0x87,0xB4,0x00,0x00,0x00,
		0x8B,0xCE,
		0xFF,0xD0,
		0xC3,
	};
	*(int*)(opcodes + 1) = pointer;
	*(int*)(opcodes + 8) = z;
	*(int*)(opcodes + 13) = y;
	*(int*)(opcodes + 18) = x;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}

/*
__asm {
	mov edx, copy_id
	mov ecx, 区域指针
	mov eax, 0xf78
	push edx
	push eax
	call __区域CALL
}
*/
void call::区域Call(PCITY_INFO city_info, int copy_id,int task_id)
{
	int 区域指针 = memory.read<int>(__区域参数);
	byte opcodes[] = {
		0xBA,0x00,0x00,0x00,0x00,
		0xB9,0x00,0x00,0x00,0x00,
		0xB8,0x78,0x0F,0x00,0x00,
		0x52,
		0x50,
		0xE8,0xEA,0xFF,0x66,0xFF,
		0xC3,
	};
	*(int*)(opcodes + 1) = copy_id;
	*(int*)(opcodes + 6) = 区域指针;
	*(int*)(opcodes + 11) = task_id;
	*(int*)(opcodes + 18) = __区域CALL - (__CALL地址+17) - 5;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
	Sleep(100);
	city_info->room.x = memory.read<int>(区域指针 + __区域偏移);
	city_info->room.y = memory.read<int>(区域指针 + __区域偏移 + 4);
	city_info->x = memory.read<int>(区域指针 + __区域偏移 + 8) + utils::createRandom(-2, 2);
	city_info->y = memory.read<int>(区域指针 + __区域偏移 + 12) + utils::createRandom(-2, 2);
}
/*
{
	mov ecx, __商店基址
	mov ecx, dword ptr[ecx]
	mov ecx, dword ptr[ecx + 0x50]
	push 0
	push 0
	push 0
	push 0
	push 0
	push 37
	push 0xff0078ff
	push Buffer
	mov eax, __喇叭公告
	call eax
}
*/
void call::公告Call(std::wstring buffer, int type, int color)
{
	byte opcodes[] = {
		0xB9,0x00,0x00,0x00,0x00,
		0x8B,0x09,
		0x8B,0x49,0x50,
		0x6A,0x00,
		0x6A,0x00,
		0x6A,0x00,
		0x6A,0x00,
		0x6A,0x00,
		0x6A,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0x68,0x00,0x00,0x00,0x00,
		0xB8,0x00,0x00,0x00,0x00,
		0xFF,0xD0,
		0xC3,
	};
	*(int*)(opcodes + 1) = __商店基址;
	*(byte*)(opcodes + 21) = type;
	*(int*)(opcodes + 23) = color;
	*(int*)(opcodes + 28) = __CALL参数;
	*(int*)(opcodes + 33) = __喇叭公告;
	//utils::mywprintf(buffer.c_str());
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes), (LPVOID)buffer.c_str(), buffer.size()*3);
}

/*
__asm
{
	mov esi, 移动指针
	mov eax, dword ptr ds : [esi]
	mov edx, dword ptr ds : [eax + __移动偏移_1]
	lea ecx, dword ptr ds : [0x400C00]
	push ecx
	mov ecx, esi
	call edx //call eax
	push 目标y
	push 目标x
	push 移动速度
	push 0x0
	sub esp, 0x8
	lea edx, dword ptr ds : [0x400C00]
	mov ecx, esp
	push edx
	call 变量入栈CALL
	mov esi, 移动指针
	mov eax, dword ptr ds : [esi]
	mov edx, dword ptr ds : [eax + __移动偏移_2]
	lea ecx, dword ptr ds : [0x400C00]
	push ecx
	mov ecx, esi
	call edx
	sub esp, 0x8
	lea eax, dword ptr ds : [0x400C00]
	mov ecx, esp
	push eax
	call 变量入栈CALL
	push 移动指针
	mov ecx, 移动指针
	call 移动CALL
}
*/
void call::移动Call(int pointer,int x,int y,int z) {
	ROLE_POS rolePos = role::getRolePos();
	int moveSpeed = abs(rolePos.x - x) + abs(rolePos.y - y);
	byte opcodes[] = {
		0xBE, 0x00,0x0C,0x40,0x00,			//mov esi, 移动指针
		0x8B, 0x06,							//mov eax, dword ptr ds : [esi]
		0x8B, 0x90, 0x9C, 0x06, 0x00, 0x00,	//mov edx, dword ptr ds : [eax + __移动偏移_1]
		0x8D, 0x0D, 0x00, 0x0C, 0x40, 0x00,	//lea ecx, dword ptr ds : [0x400C00]
		0x51,								//push ecx
		0x8B, 0xCE,							//mov ecx, esi
		0xFF, 0xD2,							//call edx //call eax	
		0x68, 0x00, 0x00, 0x00, 0x00,		//push 目标y
		0x68, 0x00, 0x00, 0x00, 0x00,		//push 目标x
		0x68, 0x00, 0x00, 0x00, 0x00,		//push 移动速度
		0x68, 0x00, 0x00, 0x00, 0x00,		//push 0x0
		0x83, 0xEC, 0x08,					//sub esp, 0x8
		0x8D, 0x15, 0x00, 0x0C, 0x40, 0x00,	//lea edx, dword ptr ds : [0x400C00]
		0x8B, 0xCC,							//mov ecx, esp
		0x52,								//push edx
		0xE8, 0xB3, 0x0B, 0x21, 0x7D,		//call 变量入栈CALL
		0xBE, 0x00, 0x0C, 0x40, 0x00,		//mov esi, 移动指针
		0x8B, 0x06,							//mov eax, dword ptr ds : [esi]
		0x8B, 0x90, 0xB0, 0x06, 0x00, 0x00,	//mov edx, dword ptr ds : [eax + __移动偏移_2]
		0x8D, 0x0D, 0x00, 0x0C, 0x40, 0x00,	//lea ecx, dword ptr ds : [0x400C00]
		0x51,								//push ecx
		0x8B, 0xCE,							//mov ecx, esi
		0xFF, 0xD2,							//call edx
		0x83, 0xEC, 0x08,					//sub esp, 0x8
		0x8D, 0x05, 0x00, 0x0C, 0x40, 0x00,	//lea eax, dword ptr ds : [0x400C00]
		0x8B, 0xCC,							//mov ecx, esp
		0x50,								//push eax
		0xE8, 0x8A, 0x0B, 0x21, 0x7D,		//call 变量入栈CALL
		0x68, 0x00, 0x0C, 0x40, 0x00,		//push 移动指针
		0xB9, 0x00, 0x0C, 0x40, 0x00,		//mov ecx, 移动指针
		0xE8, 0x7B, 0x0B, 0x21, 0x7D,		//call 移动CALL
		0xC3,
	};
	*(int*)(opcodes + 1) = pointer;
	*(int*)(opcodes + 9) = __移动偏移_1;
	*(int*)(opcodes + 15) = __空白地址;
	*(int*)(opcodes + 25) = y;
	*(int*)(opcodes + 30) = x;
	*(int*)(opcodes + 35) = moveSpeed;
	*(int*)(opcodes + 49) = __空白地址;
	*(int*)(opcodes + 57) = __变量入栈CALL - (__CALL地址 + 56) - 5;
	*(int*)(opcodes + 62) = pointer;
	*(int*)(opcodes + 70) = __移动偏移_2;
	*(int*)(opcodes + 76) = __空白地址;
	*(int*)(opcodes + 90) = __空白地址;
	*(int*)(opcodes + 98) = __变量入栈CALL - (__CALL地址 + 97) - 5;
	*(int*)(opcodes + 103) = pointer;
	*(int*)(opcodes + 108) = pointer;
	*(int*)(opcodes + 113) = __移动CALL - (__CALL地址 + 112) - 5;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}


void call::接受Call(int taskId)
{
	byte opcodes[] = {
		0x68,0xD4,0x22,0x00,0x00,
		0xE8,0x06,0x8B,0xAF,0xF7,
		0xC7,0x44,0x24,0xFC,0x00,0x00,0x00,0x00,
		0xC3
	};
	*(int*)(opcodes + 1) = taskId;
	*(int*)(opcodes + 6) = __接受CALL - (__CALL地址 + 5) -5;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}

void call::完成Call(int taskId)
{
	byte opcodes[] = {
		0x6A, 0xFF,           		//push -01 { 255 }
		0x68, 0xFF,0x00,0x00,0x00,  //push 000000FF { 255 }         
		0x68, 0x09,0x13,0x00,0x00,  //push 00001309 { 4873 }         
		0xB8, 0x10,0x8A,0xEF,0x01,  //mov eax,dnf.exe+1AF8A10 { [E8EC8B55] }         
		0xFF, 0xD0,           		//call eax
		0xC3           				//ret 
	};
	*(int*)(opcodes + 8) = taskId;
	*(int*)(opcodes + 13) = __完成CALL;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}

void call::提交Call(int taskId)
{
	byte opcodes[] = {
		0x6A, 0x01,          							//push 01 { 1 }
		0x68, 0xFF,0xFF,0x00,0x00,           			//push 0000FFFF { 65535 }
		0x68, 0x46,0x31,0x00,0x00,           			//push 00003146 { 12614 }
		0xB9, 0xA8,0x43,0x69,0x05,           			//mov ecx,dnf.exe+52943A8 { [55E2D600] }
		0xB8, 0x90,0x8A,0xEF,0x01,           			//mov eax,dnf.exe+1AF8A90 { [E8EC8B55] }
		0xFF, 0xD0,        								//call eax
		0xC7, 0x44,0x24 ,0xFC, 0x00,0x00,0x00,0x00, //mov [esp-04],00000000 { 0 }
		0xC3						           			//ret 
	};
	*(int*)(opcodes + 8) = taskId;
	*(int*)(opcodes + 13) = __任务基址;
	*(int*)(opcodes + 18) = __提交CALL;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}

void call::放弃Call(int taskId)
{

}

void call::跳过Call() {
	byte opcodes[] = {
		0x60,                    			 //pushad 
		0x8B, 0x0D,0xA8,0x43,0x69,0x05,    //mov ecx,[dnf.exe+52943A8] { [55E2D600] }
		0x6A, 0xFF,                		 //push -01 { 255 }
		0x6A, 0x01,                		 //push 01 { 1 }
		0xE8, 0xD0,0xC4,0xA4,0xFA,          //call dnf.exe+188C4E0
		0x61,                    			 //popad 
		0xC3             					//ret 
	};
	*(int*)(opcodes + 3) = __任务基址;
	*(int*)(opcodes + 12) = __跳过CALL - (__CALL地址 + 11) - 5;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}

void call::变身Call(int 怪物代码) 
{
	
	int param_struct[10] = {};
	param_struct[0] = 怪物代码;
	param_struct[1] = 70;
	param_struct[2] = 70;
	param_struct[3] = -1;
	param_struct[4] = 1;
	byte opcodes[] = {
		0x68, 0x00,0x00,0xF9,0x06,            //push bs_struct { [0001AFBD] }
		0xB8, 0x88,0x7B,0x7A,0x05,            //mov eax,dnf.exe+53A7B88 { [72A60000] }
		0x8B, 0x00,          					//mov eax,[eax]
		0x50,            						//push eax
		0xB9, 0xB0,0xB0,0x6B,0x05,            //mov ecx,dnf.exe+52BB0B0 { [00000000] }
		0x8B, 0x09,           				//mov ecx,[ecx]
		0xB8, 0x50,0x1D,0x5D,0x02,            //mov eax,dnf.exe+21D1D50 { [325FE2E9] }
		0xFF, 0xD0,           				//call eax
		0xC3                    				//ret 
	};
	*(int*)(opcodes + 1) = __CALL参数;
	*(int*)(opcodes + 6) = __人物基址;
	*(int*)(opcodes + 14) = __召唤参数;
	*(int*)(opcodes + 21) = __召唤怪物CALL;

	fun::remoteMainThreadCall(opcodes, sizeof(opcodes), param_struct, sizeof(param_struct));
	
}

void call::过图Call(byte direction) {
	byte opcodes[] = {
		0x60,             							//pushad 	
		0x8B, 0x0D, 0x68,0x43,0x69,0x05,              //mov ecx,[dnf.exe+5294368] { [56130700] }	
		0x8B, 0x89, 0x28,0xA0,0x20,0x00,        		//mov ecx,[ecx+0020A028]
		0x8B, 0x89, 0x8C,0x00,0x00,0x00,        		//mov ecx,[ecx+0000008C]
		0x6A, 0xFF,                 					//push -01 { 255 }
		0x6A, 0xFF,                 					//push -01 { 255 }
		0x6A, 0x00,                 					//push 00 { 0 }
		0x6A, 0x00,                 					//push 00 { 0 }
		0x6A, 0x00,                 					//push 00 { 0 }
		0x6A, 0x00,                 					//push 00 { 0 }
		0x6A, 0x00,                 					//push 00 { 0 }
		0x6A, 0x01,                 					//push 01 { 1 }
		0xB8, 0x00, 0x3C,0xA6,0x01,           		//mov eax,dnf.exe+1663C00 { [6AEC8B55] }
		0xFF, 0xD0,                 					//call eax
		0xC7, 0x44, 0x24, 0x04, 0x00,0x00,0x00,0x00,  //mov [esp+04],00000000 { 0 }
		0x61,                 						//popad 
		0xC3                 							//ret 
	};
	*(int*)(opcodes + 3) = __房间编号;
	*(int*)(opcodes + 9) = __时间基址;
	*(byte*)(opcodes + 34) = direction;
	*(int*)(opcodes + 36) = __过图CALL;
	fun::remoteMainThreadCall(opcodes, sizeof(opcodes));
}