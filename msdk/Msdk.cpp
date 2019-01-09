#include "msdk.h"

#ifdef _WIN64
#ifndef __UTILS_H__
#include "../utils/utils.h"
#pragma comment(lib,"../x64/Release/library/utils.lib")
#endif

#ifndef __MEMORY_LOADER_H__
#include "../memory_loader/memory_loader.h"
#pragma comment(lib,"../x64/Release/library/memory_loader.lib")
#endif // !__MEMORY_LOADER_H__
#else _WIN64
#ifndef __UTILS_H__
#include "../utils/utils.h"
#pragma comment(lib,"../Release/library/utils.lib")
#endif

#ifndef __MEMORY_LOADER_H__
#include "../memory_loader/memory_loader.h"
#pragma comment(lib,"../Release/library/memory_loader.lib")
#endif // !__MEMORY_LOADER_H__
#endif // WIN64

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


Msdk::Msdk() {
	Init();
}

Msdk::~Msdk() {
	closeHandle();
}

void Msdk::Init() 
{
	FILE *fp;
	unsigned char *data = NULL;
	size_t size;
	HMEMORYMODULE module = NULL;
	LPCSTR path = VMProtectDecryptStringA("./dll/msdk.dat");
	fopen_s(&fp, path, "rb");
	if (fp == NULL)
	{
		printf(VMProtectDecryptStringA("打开文件失败\n"));
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
		printf(VMProtectDecryptStringA("获取模块失败\n"));
		return;
	}
	M_Open_VidPid = (M_OPEN_VIDPID)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_Open_VidPid"));
	M_ReleaseAllKey = (M_RELEASEALLKEY)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_ReleaseAllKey"));
	M_KeyState2 = (M_KEYSTATE2)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_KeyState2"));
	M_KeyDown2 = (M_KEYDOWN2)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_KeyDown2"));
	M_KeyUp2 = (M_KEYUP2)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_KeyUp2"));
	M_LeftClick = (M_LEFTCLICK)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_LeftClick"));
	M_MoveTo2 = (M_MOVETO2)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_MoveTo2"));
	M_MoveTo3 = (M_MOVETO3)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_MoveTo3"));
	M_GetCurrMousePos2 = (M_GETCURMOUSEPOS2)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_GetCurrMousePos2"));
	M_Close = (M_CLOSE)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_Close"));
	M_ResolutionUsed = (M_RESOLUTIONUSED)MemoryGetProcAddress(module, VMProtectDecryptStringA("M_ResolutionUsed"));

	openHandle();
}


void Msdk::openHandle()
{
	if (msdk_handle == NULL)
	{
		msdk_handle = M_Open_VidPid(0xc310, 0xc007);
		if (msdk_handle == INVALID_HANDLE_VALUE) {
			printf(VMProtectDecryptStringA("msdk 打开失败\n"));
		}

		M_ResolutionUsed(msdk_handle, 1920, 1080);
	}
}

void Msdk::closeHandle()
{
	if (msdk_handle != NULL)
	{
		M_Close(msdk_handle);
		msdk_handle = NULL;
	}

}

void Msdk::upAllKey()
{
	M_ReleaseAllKey(msdk_handle);
}

int Msdk::getKeyState(int keyCode)
{
	int keyState = M_KeyState2(msdk_handle, keyCode);
	//printf("keyState->:%d\n", keyState);
	return keyState;
}

void Msdk::keyDown(int keyCode)
{
	if (getKeyState(keyCode) == 0)
	{
		M_KeyDown2(msdk_handle, keyCode);
		Sleep(105 + utils::createRandom(1, 10));
		/*if (getKeyState(keyCode) == 1)
		{
			M_KeyDown2(msdk_handle, keyCode);
			Sleep(100);
		}*/
	}
	/*else {
		M_KeyUp2(msdk_handle, keyCode);
	}*/

}
void Msdk::keyUp(int keyCode)
{
	if (getKeyState(keyCode) == 1)
	{
		M_KeyUp2(msdk_handle, keyCode);
		Sleep(50+ utils::createRandom(1, 10));
		/*if (getKeyState(keyCode) == 0) {
			M_KeyUp2(msdk_handle, keyCode);
			Sleep(100);
		}*/
	}
	/*else {
		M_KeyDown2(msdk_handle, keyCode);
	}*/
}
void Msdk::doKeyPress(int keyCode, int s)
{
	keyDown(keyCode);
	Sleep(s + utils::createRandom(1, 10));
	keyUp(keyCode);
}
void Msdk::mouseClick()
{
	M_LeftClick(msdk_handle, 1);
	Sleep(100);
}
void Msdk::mouseDoubleClick(int s)
{
	mouseClick();
	Sleep(s + utils::createRandom(0, 10));
	mouseClick();
}
void Msdk::moveMousePos(int x, int y)
{
	M_MoveTo2(msdk_handle, x, y);
}
void Msdk::setMousePos(int x, int y)
{
	M_MoveTo3(msdk_handle, x, y);
}
void Msdk::getMousePos(int *x, int *y)
{
	M_GetCurrMousePos2(x, y);
}