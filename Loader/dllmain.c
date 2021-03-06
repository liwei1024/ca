// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "header.h"
#include "memory_loader.h"
#include <direct.h>

HMODULE g_hDll = NULL;

HMODULE g_hModule;


typedef HANDLE(WINAPI*M_OPEN_VIDPID)(int, int);
typedef int (WINAPI*M_RELEASEALLKEY)(HANDLE);
typedef int (WINAPI*M_KEYSTATE2)(HANDLE, int);
typedef int (WINAPI*M_KEYDOWN2)(HANDLE, int);
typedef int (WINAPI*M_KEYUP2)(HANDLE, int);
typedef int (WINAPI*M_LEFTCLICK)(HANDLE, int);
typedef int (WINAPI*M_MOVETO2)(HANDLE, int, int);
typedef int (WINAPI*M_MOVETO3)(HANDLE, int, int);
typedef int (WINAPI*M_GETCURMOUSEPOS2)(int*, int*);
typedef int (WINAPI*M_CLOSE)(HANDLE);
typedef int (WINAPI*M_RESOLUTIONUSED)(HANDLE, int, int);

M_OPEN_VIDPID M_Open_VidPid;
M_RELEASEALLKEY M_ReleaseAllKey;
M_KEYSTATE2 M_KeyState2;
M_KEYDOWN2 M_KeyDown2;
M_KEYUP2 M_KeyUp2;
M_LEFTCLICK M_LeftClick;
M_MOVETO2 M_MoveTo2;
M_MOVETO3 M_MoveTo3;
M_GETCURMOUSEPOS2 M_GetCurrMousePos2;
M_CLOSE M_Close;
M_RESOLUTIONUSED M_ResolutionUsed;

typedef VOID(*pEntryPoint)(
	M_OPEN_VIDPID, 
	M_RELEASEALLKEY, 
	M_KEYSTATE2, 
	M_KEYDOWN2, 
	M_KEYUP2,
	M_LEFTCLICK, 
	M_MOVETO2,
	M_MOVETO3,
	M_GETCURMOUSEPOS2,
	M_CLOSE,
	M_RESOLUTIONUSED
);

char szFilePath[MAX_PATH + 1] = { 0 };

BOOL LoadFromMemory1()
{
	FILE *fp;
	unsigned char *data = NULL;
	size_t size;
	HMEMORYMODULE module;

	//fopen_s(&fp, "C:\\Users\\lw\\source\\repos\\liwei1024\\WC-2\\Release\\KeyCall.dat", "rb");
	fopen_s(&fp, "C:\\msdk.dat", "rb");
	if (fp == NULL)
	{
		MessageBox(NULL, L"", (L"MSDK"), MB_OK);
		return FALSE;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	data = (unsigned char *)malloc(size);
	fseek(fp, 0, SEEK_SET);
	fread(data, 1, size, fp);
	fclose(fp);
	module = MemoryLoadLibrary(data);
	if (module == NULL)
	{
		return FALSE;
	}

	M_Open_VidPid = (M_OPEN_VIDPID)MemoryGetProcAddress(module, "M_Open_VidPid");
	M_ReleaseAllKey = (M_RELEASEALLKEY)MemoryGetProcAddress(module, "M_ReleaseAllKey");
	M_KeyState2 = (M_KEYSTATE2)MemoryGetProcAddress(module, "M_KeyState2");
	M_KeyDown2 = (M_KEYDOWN2)MemoryGetProcAddress(module, "M_KeyDown2");
	M_KeyUp2 = (M_KEYUP2)MemoryGetProcAddress(module, "M_KeyUp2");
	M_LeftClick = (M_LEFTCLICK)MemoryGetProcAddress(module, "M_LeftClick");
	M_MoveTo2 = (M_MOVETO2)MemoryGetProcAddress(module, "M_MoveTo2");
	M_MoveTo3 = (M_MOVETO3)MemoryGetProcAddress(module, "M_MoveTo3");
	M_GetCurrMousePos2 = (M_GETCURMOUSEPOS2)MemoryGetProcAddress(module, "M_GetCurrMousePos2");
	M_Close = (M_CLOSE)MemoryGetProcAddress(module, "M_Close");
	M_ResolutionUsed = (M_RESOLUTIONUSED)MemoryGetProcAddress(module, "M_ResolutionUsed");

	return TRUE;
}

void LoadFromMemory2()
{
	FILE *fp;
	unsigned char *data = NULL;
	size_t size;
	HMEMORYMODULE module;
	pEntryPoint EntryPoint;

	//fopen_s(&fp, "C:\\Users\\lw\\source\\repos\\liwei1024\\WC-2\\Release\\dsrole.dat", "rb");
	fopen_s(&fp, "C:\\dsrole.dat", "rb");
	if (fp == NULL)
	{
		MessageBox(NULL, L"", (L"WC dsrole load fail"), MB_OK);
		return;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	data = (unsigned char *)malloc(size);
	fseek(fp, 0, SEEK_SET);
	fread(data, 1, size, fp);
	fclose(fp);

	module = MemoryLoadLibrary(data);
	if (module == NULL)
	{
		return;
	}
	EntryPoint = (pEntryPoint)MemoryGetProcAddress(module, "EntryPoint");
	EntryPoint(
		M_Open_VidPid,
		M_ReleaseAllKey,
		M_KeyState2,
		M_KeyDown2,
		M_KeyUp2,
		M_LeftClick,
		M_MoveTo2,
		M_MoveTo3,
		M_GetCurrMousePos2,
		M_Close,
		M_ResolutionUsed
	);
}

//void remover_self()
//{
//	char szBuff[MAX_PATH] = { 0 };
//	GetModuleFileNameA(g_hModule, szBuff, MAX_PATH);
//	sprintf_s(szBuff, MAX_PATH,"WC %s", szBuff);
//	OutputDebugStringA(szBuff);
//	remove(szBuff);
//}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//DisableThreadLibraryCalls(hModule);//禁用指定的DLL的DLL_THREAD_ATTACH和DLL_THREAD_DETACH通知，这样可以减小某些程序的工作集大小。
		//g_hDll = LoadLibrary(_T("C:\\Windows\\System32\\dsrole.dll"));
		g_hModule = hModule;
		if (LoadFromMemory1() == TRUE) {
			LoadFromMemory2();
			//OutputDebugString(L"WC 加载");
		}
		//CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)remover_self,NULL,0,0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return FALSE;
}


