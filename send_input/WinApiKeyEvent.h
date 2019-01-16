#pragma once
#include <windows.h>
class WinApiKeyEvent
{
public:
	WinApiKeyEvent();
	~WinApiKeyEvent();
	bool getKeyState(int keyCode);
	void keyDown(int keyCode, int s = 100);
	void keyUp(int keyCode);
	void doKeyPress(int keyCode, int s);
private:
	WORD getSCan(int keyCode);
};

