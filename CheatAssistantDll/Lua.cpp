#include "stdafx.h"
#include "Lua.h"


Lua::Lua()
{
	L = luaL_newstate();
	luaL_openlibs(L); //‘ÿ»ÎLuaª˘±æø‚

	registers(L);
}
Lua::~Lua()
{
	lua_close(L);
}
void Lua::registers(lua_State* m_L)
{
	/*lua_pushcfunction(m_L, getForegroundWindow);
	lua_setglobal(m_L,"getForegroundWindow");*/
	// memory
	lua_register(m_L, "readByte", readByte); // 
	lua_register(m_L, "readShort", readShort); // 
	lua_register(m_L, "readInteger", readInteger); // 
	lua_register(m_L, "readFloat", readFloat); // 
	lua_register(m_L, "readDouble", readDouble); // 
	lua_register(m_L, "readString", readString); // 
	lua_register(m_L, "readWString", readWString); // 
	lua_register(m_L, "writeByte", writeByte); // 
	lua_register(m_L, "writeShort", writeShort); // 
	lua_register(m_L, "writeInteger", writeInteger); // 
	lua_register(m_L, "writeFloat", writeFloat); // 
	lua_register(m_L, "writeDouble", writeDouble); // 
	// winapi
	lua_register(m_L, "findWindow", findWindow); // 
	lua_register(m_L, "getForegroundWindow", getForegroundWindow); // 
	lua_register(m_L, "setForegroundWindow", setForegroundWindow); // 
	lua_register(m_L, "setWindowLong", setWindowLong); // 
	lua_register(m_L, "outputDebugString", outputDebugString); // 
	lua_register(m_L, "messageBox", messageBox); // 
	lua_register(m_L, "sendMessage", sendMessage); // 
	// key
	lua_register(m_L, "keyUp", keyUp); // 
	lua_register(m_L, "keyDown", keyDown); // 
	lua_register(m_L, "doKeyPress", doKeyPress); // 
	lua_register(m_L, "upAllKey", upAllKey); // 
	lua_register(m_L, "getKeyState", getKeyState); // 
	lua_register(m_L, "mouseClick", mouseClick); // 
	lua_register(m_L, "mouseDoubleClick", mouseDoubleClick); // 
	lua_register(m_L, "moveMousePos", moveMousePos); // 
	lua_register(m_L, "setMousePos", setMousePos); // 
	lua_register(m_L, "getMousePos", getMousePos); // 
	
}
bool  Lua::check(int result)
{
	char buffer[1024];
	if (result == LUA_OK) {
		return true;
	}
	sprintf_s(buffer,"Lua %s", lua_tostring(L, -1));
	OutputDebugStringA(buffer);
	MessageBoxA(NULL, buffer,"CA",MB_OK);
	return false;
}
bool Lua::doFile(const char * filePath) {
	return check(luaL_dofile(L, filePath));
}
void Lua::getGlobal(const char * name)
{
	lua_getglobal(L, name);
}
// memory
int Lua::readByte(lua_State* m_L)
{
	lua_pushinteger(m_L, memory::read<byte>((DWORD_PTR)lua_tointeger(m_L, 1)));
	return 1;
}
int Lua::readShort(lua_State* m_L)
{
	lua_pushinteger(m_L, memory::read<short>((DWORD_PTR)lua_tointeger(m_L, 1)));
	return 1;
}
int Lua::readInteger(lua_State* m_L)
{
	lua_pushinteger(m_L, memory::read<int>((DWORD_PTR)lua_tointeger(m_L, 1)));
	return 1;
}
int Lua::readLong(lua_State* m_L)
{
	lua_pushinteger(m_L, memory::read<LONGLONG>((DWORD_PTR)lua_tointeger(m_L, 1)));
	return 1;
}
int Lua::readFloat(lua_State* m_L)
{
	lua_pushnumber(m_L, memory::read<float>((DWORD_PTR)lua_tonumber(m_L, 1)));
	return 1;
}
int Lua::readDouble(lua_State* m_L)
{
	lua_pushnumber(m_L, memory::read<double>((DWORD_PTR)lua_tonumber(m_L, 1)));
	return 1;
}
int Lua::readString(lua_State* m_L)
{
	lua_pushstring(m_L, memory::readString((DWORD_PTR)lua_tonumber(m_L, 1), (size_t)lua_tointeger(m_L, 2)).c_str());
	return 1;
}
int Lua::readWString(lua_State* m_L)
{
	lua_pushstring(m_L, utils::wcharToChar(memory::readWString((DWORD_PTR)lua_tonumber(m_L, 1), (size_t)lua_tointeger(m_L, 2)).c_str()).c_str());
	return 1;
}
int Lua::writeByte(lua_State* m_L)
{
	lua_pushboolean(m_L, memory::write<byte>((DWORD_PTR)lua_tointeger(m_L, 1), (byte)lua_tointeger(m_L, 2)));
	return 1;
}
int Lua::writeShort(lua_State* m_L)
{
	lua_pushboolean(m_L, memory::write<short>((DWORD_PTR)lua_tointeger(m_L, 1), (short)lua_tointeger(m_L, 2)));
	return 1;
}
int Lua::writeInteger(lua_State* m_L)
{
	lua_pushboolean(m_L, memory::write<int>((DWORD_PTR)lua_tointeger(m_L, 1), (int)lua_tointeger(m_L, 2)));
	return 1;
}
int Lua::writeLong(lua_State* m_L)
{
	lua_pushboolean(m_L, memory::write<LONGLONG>((DWORD_PTR)lua_tointeger(m_L, 1), (LONGLONG)lua_tonumber(m_L, 2)));
	return 1;
}
int Lua::writeFloat(lua_State* m_L)
{
	lua_pushboolean(m_L, memory::write<float>((DWORD_PTR)lua_tointeger(m_L, 1), (float)lua_tonumber(m_L, 2)));
	return 1;
}
int Lua::writeDouble(lua_State* m_L)
{
	lua_pushboolean(m_L, memory::write<double>((DWORD_PTR)lua_tointeger(m_L, 1), (double)lua_tonumber(m_L, 2)));
	return 1;
}
// winapi
int Lua::findWindow(lua_State* m_L)
{
	LPCSTR lpClassName = lua_tostring(m_L, 1) ? lua_tostring(m_L, 1) : "";
	LPCSTR lpWindowName = lua_tostring(m_L, 2) ? lua_tostring(m_L, 2) : "";
	HWND hWnd = FindWindowA(lpClassName, lpWindowName);
	if (hWnd == NULL)
	{
		lua_pushnil(m_L);
	}
	else {
		lua_pushinteger(m_L, (lua_Integer)hWnd);
	}
	return 1;
}
int Lua::getForegroundWindow(lua_State* m_L)
{
	lua_pushinteger(m_L, (lua_Integer)GetForegroundWindow());
	return 1;
}
int Lua::setForegroundWindow(lua_State* m_L)
{
	lua_pushboolean(m_L,SetForegroundWindow((HWND)lua_tointeger(m_L,1)));
	return 1;
}
int Lua::setWindowLong(lua_State* m_L)
{
	HWND hWnd = (HWND)lua_tointeger(m_L, 1);
	int index = (int)lua_tointeger(m_L, 2);
	LONG newLong = (LONG)lua_tointeger(m_L,3);
	lua_pushinteger(m_L,SetWindowLongW(hWnd, index, newLong));
	return 1;
}
int Lua::outputDebugString(lua_State* m_L)
{
	OutputDebugStringA(lua_tostring(m_L,1));
	return 0;
}
int Lua::messageBox(lua_State* m_L)
{
	MessageBoxA(NULL, lua_tostring(m_L, 1), "CA", MB_OK);
	return 0;
}
int Lua::sendMessage(lua_State* m_L) 
{
	HWND hWnd = (HWND)lua_tointeger(m_L, 1);
	UINT uMsg = (UINT)lua_tointeger(m_L, 2);
	WPARAM wParam = (WPARAM)lua_tointeger(m_L, 3);
	LPARAM lParam = (LPARAM)lua_tointeger(m_L, 4);
	lua_pushinteger(m_L,(lua_Integer)SendMessageW(hWnd, uMsg, wParam, lParam));
	return 1;
}
// key
int Lua::keyUp(lua_State* m_L) {
	int keyCode = (int)lua_tointeger(m_L, 1);
	if (M_KeyState2(g_msdkHandle, keyCode) == 1)
	{
		M_KeyUp2(g_msdkHandle, keyCode);
		Sleep(100);
	}
	return 0;
}
int Lua::keyDown(lua_State* m_L) {
	int keyCode = (int)lua_tointeger(m_L, 1);
	if (M_KeyState2(g_msdkHandle, keyCode) == 0)
	{
		M_KeyDown2(g_msdkHandle, keyCode);
		Sleep(100);
	}
	return 0;
}
int Lua::doKeyPress(lua_State* m_L)
{
	int keyCode = (int)lua_tointeger(m_L, 1);
	int s = 100;
	if (lua_isnil(m_L,2) == 0) {
		s += (int)lua_tointeger(m_L, 2);
	}
	M_KeyDown2(g_msdkHandle, keyCode);
	Sleep(s + utils::createRandom(1, 10));
	M_KeyUp2(g_msdkHandle, keyCode);
	Sleep(100);
	return 0;
}
int Lua::upAllKey(lua_State* m_L)
{
	M_ReleaseAllKey(g_msdkHandle);
	return 0;
}
int Lua::getKeyState(lua_State* m_L)
{
	lua_pushinteger(m_L,M_KeyState2(g_msdkHandle, (int)lua_tointeger(m_L, 1)));
	return 1;
}
int Lua::mouseClick(lua_State* m_L)
{
	M_LeftClick(g_msdkHandle, 1);
	Sleep(100);
	return 0;
}
int Lua::mouseDoubleClick(lua_State* m_L)
{
	M_LeftClick(g_msdkHandle, 1);
	Sleep(100 + utils::createRandom(0, 10));
	M_LeftClick(g_msdkHandle, 1);
	return 0;
}
int Lua::moveMousePos(lua_State* m_L)
{
	int x = (int)lua_tointeger(m_L, 1);
	int y = (int)lua_tointeger(m_L, 2);
	M_MoveTo2(g_msdkHandle, x, y);
	return 0;
}
int Lua::setMousePos(lua_State* m_L)
{
	int x = (int)lua_tointeger(m_L, 1);
	int y = (int)lua_tointeger(m_L, 2);
	M_MoveTo3(g_msdkHandle, x, y);
	return 0;
}
int Lua::getMousePos(lua_State* m_L)
{
	int x, y = 0;
	M_GetCurrMousePos2(&x, &y);
	lua_pushinteger(m_L, x);
	lua_pushinteger(m_L, y);
	return 2;
}