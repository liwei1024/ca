#include "header.h"

// 导出函数
#pragma comment(linker, "/EXPORT:DsRoleFreeMemory=_JMP_DsRoleFreeMemory,@1")
#pragma comment(linker, "/EXPORT:DsRoleGetPrimaryDomainInformation=_JMP_DsRoleGetPrimaryDomainInformation,@2")
// 宏定义
#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)
#define ALCPP EXPORT NAKED
#define ALSTD  EXPORT NAKED void __stdcall
#define ALCFAST  EXPORT NAKED void __fastcall
#define ALCDECL  NAKED void __cdecl
//全局变量

DWORD dwRetaddress[3];                          //存放返回地址

// 内部函数 获取真实函数地址
static TCHAR szTemp[MAX_PATH] = { 0 };
static inline FARPROC WINAPI GetAddress(PCSTR pszProcName)
{
	//VMProtectBegin("GetAddress");
	FARPROC fpAddress;
	fpAddress = GetProcAddress(g_hDll, pszProcName);
	if (fpAddress == NULL)
	{
		swprintf_s(szTemp, MAX_PATH, L"WC 无法找到函数 :%hs 的地址 ", pszProcName);
		MessageBox(NULL, szTemp, L"错误", MB_OK);
		ExitProcess(-2);
	}
	//返回真实地址
	return fpAddress;
	//VMProtectEnd();
}

ALCDECL JMP_DsRoleFreeMemory()
{
	//以下注释经过OD调试得出 编译环境:win10 x64 vs2013， 
			//一般情况下在这里为所欲为   注意堆栈平衡
	GetAddress("DsRoleFreeMemory");
	//此时栈订保持的是返回地址,因为我们前面没有破坏堆栈
	__asm pop dwRetaddress[1]                       //弹出来，下面菜可以用call,为什么用call？因为如果用直接jmp的话 想获取执行返回值有点困难
		__asm call eax                              //把返回地址入栈，这时候就相当于原来的返回地址被我们call的下一条指令地址入栈，这样真实函数返回后我们重新夺回控制权
		//一般情况下在这里继续为所欲为  注意堆栈平衡
	__asm jmp dword ptr dwRetaddress[1]         //跳回原函数
}

ALCDECL JMP_DsRoleGetPrimaryDomainInformation()
{
	//以下注释经过OD调试得出 编译环境:win10 x64 vs2013， 
			//一般情况下在这里为所欲为   注意堆栈平衡
	GetAddress("DsRoleGetPrimaryDomainInformation");
	//此时栈订保持的是返回地址,因为我们前面没有破坏堆栈
	__asm pop dwRetaddress[2]                       //弹出来，下面菜可以用call,为什么用call？因为如果用直接jmp的话 想获取执行返回值有点困难
		__asm call eax                              //把返回地址入栈，这时候就相当于原来的返回地址被我们call的下一条指令地址入栈，这样真实函数返回后我们重新夺回控制权
		//一般情况下在这里继续为所欲为  注意堆栈平衡
	__asm jmp dword ptr dwRetaddress[2]         //跳回原函数
}