#include "pch.h"
#include "hook.h"


Hook::Hook()
{
}


Hook::~Hook()
{
}

void Hook::jmpHook(int targetAddress,int jmpAddress,size_t hookLength)
{
	g_targetAddress = targetAddress;

	g_oldData = memory.readBytes(targetAddress, hookLength);
	for (size_t i = 0; i < hookLength; i++)
	{
		printf("%x ", g_oldData[i]);

	}
	printf("\n");
	byte *jmp_shell_code = new byte[hookLength];

	*(byte*)(jmp_shell_code) = 0xe9;

	*(int*)(jmp_shell_code + 1) = jmpAddress - targetAddress - 5;

	for (size_t i = 5; i < hookLength; i++)
	{
		*(byte*)(jmp_shell_code + i) = g_oldData[i];
	}

	memory.writeVirtualMemory(targetAddress, jmp_shell_code, sizeof(jmp_shell_code));
}

void Hook::unHook() 
{
	if (g_oldData.size() > 0)
	{
		memory.writeBytes(g_targetAddress, g_oldData);
	}
}


//void Hook::jmpHook2(int targetAddress,int jmpAddress, size_t hookLength,std::vector<byte>hookCode) 
//{
//	// 保存原地址数据
//	g_oldData = memory.readBytes(targetAddress, hookLength);
//	hookCode.resize(hookCode.size() + hookLength);
//	// 写入hook代码
//	//memory.write(jmpAddress, hookCode);
//}