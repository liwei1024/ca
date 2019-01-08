#pragma once
#ifdef __cplusplus
extern "C" {
#include "../lua53/lua.h"
#include "../lua53/lualib.h"  
#include "../lua53/lauxlib.h" 
#pragma comment(lib,"../Release/library/lua53.lib")

}
#else  
#include "../lua53/lua.hpp"
#include "../lua53/lualib.h"  
#include "../lua53/lauxlib.h"  
#pragma comment(lib,"../Release/library/lua53.lib")
#endif

#ifndef __UTILS_H__
#include "../utils/utils.h"
#pragma comment(lib,"../Release/library/utils.lib")
#endif // !__UTILS_H__

class Lua
{
public:
	lua_State * L = NULL;
	Lua();
	~Lua();
	void registers(lua_State * m_L);
	bool check(int result);
	bool doFile(const char * filePath);
	void getGlobal(const char * name);
	static int readByte(lua_State * m_L);
	static int readShort(lua_State * m_L);
	static int readInteger(lua_State * m_L);
	static int readLong(lua_State * m_L);
	static int readFloat(lua_State * m_L);
	static int readDouble(lua_State * m_L);
	static int readString(lua_State * m_L);
	static int readWString(lua_State * m_L);
	static int writeByte(lua_State * m_L);
	static int writeShort(lua_State * m_L);
	static int writeInteger(lua_State * m_L);
	static int writeLong(lua_State * m_L);
	static int writeFloat(lua_State * m_L);
	static int writeDouble(lua_State * m_L);
	static int findWindow(lua_State * m_L);
	static int getForegroundWindow(lua_State * m_L);
	static int setForegroundWindow(lua_State * m_L);
	static int setWindowLong(lua_State * m_L);
	static int outputDebugString(lua_State * m_L);
	static int messageBox(lua_State * m_L);
	static int sendMessage(lua_State * m_L);
	static int keyUp(lua_State * m_L);
	static int keyDown(lua_State * m_L);
	static int doKeyPress(lua_State * m_L);
	static int upAllKey(lua_State * m_L);
	static int getKeyState(lua_State * m_L);
	static int mouseClick(lua_State * m_L);
	static int mouseDoubleClick(lua_State * m_L);
	static int moveMousePos(lua_State * m_L);
	static int setMousePos(lua_State * m_L);
	static int getMousePos(lua_State * m_L);
};

