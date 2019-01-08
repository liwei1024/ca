#ifndef __LUA_H__
#define __LUA_H__
#ifdef __cplusplus
extern "C" {
#include "../lua53/lua.h"
#include "../lua53/lualib.h"  
#include "../lua53/lauxlib.h" 
#pragma comment(lib,"../x64/Release/library/lua53.lib")

}
#else  
#include "../lua53/lua.hpp"
#include "../lua53/lualib.h"  
#include "../lua53/lauxlib.h"  
#pragma comment(lib,"../x64/Release/library/lua53.lib")
#endif

#ifndef __UTILS_H__
#include "../utils/utils.h"
#pragma comment(lib,"../x64/Release/library/utils.lib")
#endif // !__UTILS_H__



class Lua
{
public:
	lua_State* L = NULL;
	Lua() {
		L = luaL_newstate();
		luaL_openlibs(L); //‘ÿ»ÎLuaª˘±æø‚
		// memory
		lua_register(L, "readByte", readByte); // 
		lua_register(L, "readShort", readShort); // 
		lua_register(L, "readInteger", readInteger); // 
		lua_register(L, "readFloat", readFloat); // 
		lua_register(L, "readDouble", readDouble); // 
		lua_register(L, "writeByte", writeByte); // 
		lua_register(L, "writeShort", writeShort); // 
		lua_register(L, "writeInteger", writeInteger); // 
		lua_register(L, "writeFloat", writeFloat); // 
		lua_register(L, "writeDouble", writeDouble); // 
		// http
		//lua_register(L, "httpGet", httpGet); // 
		//lua_register(L, "httpPost", httpPost); // 
	}
	~Lua() {
		lua_close(L);
	}
	bool check(int result)
	{
		if (result == LUA_OK) {
			return true;
		}
		utils::myprintf("%s", RED, lua_tostring(L, -1));
		return false;
	}
	void doFile(const char * filePath) {
		check(luaL_dofile(L, filePath));
	}
	// memory
	static int readByte(lua_State* m_L)
	{
		lua_pushinteger(m_L, memory.read<byte>((DWORD_PTR)lua_tointeger(m_L, 1)));
		return 1;
	}

	static int readShort(lua_State* m_L)
	{
		lua_pushinteger(m_L, memory.read<short>((DWORD_PTR)lua_tointeger(m_L, 1)));
		return 1;
	}

	static int readInteger(lua_State* m_L)
	{
		lua_pushinteger(m_L, memory.read<int>((DWORD_PTR)lua_tointeger(m_L, 1)));
		return 1;
	}

	static int readLong(lua_State* m_L)
	{
		lua_pushinteger(m_L, memory.read<LONGLONG>((DWORD_PTR)lua_tointeger(m_L, 1)));
		return 1;
	}

	static int readFloat(lua_State* m_L)
	{
		lua_pushnumber(m_L, memory.read<float>((DWORD_PTR)lua_tonumber(m_L, 1)));
		return 1;
	}
	static int readDouble(lua_State* m_L)
	{
		lua_pushnumber(m_L, memory.read<double>((DWORD_PTR)lua_tonumber(m_L, 1)));
		return 1;
	}

	static int writeByte(lua_State* m_L)
	{
		lua_pushboolean(m_L, memory.write<byte>((DWORD_PTR)lua_tointeger(m_L, 1), (byte)lua_tointeger(m_L, 2)));
		return 1;
	}
	static int writeShort(lua_State* m_L)
	{
		lua_pushboolean(m_L, memory.write<short>((DWORD_PTR)lua_tointeger(m_L, 1), (short)lua_tointeger(m_L, 2)));
		return 1;
	}
	static int writeInteger(lua_State* m_L)
	{
		lua_pushboolean(m_L, memory.write<int>((DWORD_PTR)lua_tointeger(m_L, 1), (int)lua_tointeger(m_L, 2)));
		return 1;
	}
	static int writeLong(lua_State* m_L)
	{
		lua_pushboolean(m_L, memory.write<LONGLONG>((DWORD_PTR)lua_tointeger(m_L, 1), (LONGLONG)lua_tonumber(m_L, 2)));
		return 1;
	}
	static int writeFloat(lua_State* m_L)
	{
		lua_pushboolean(m_L, memory.write<float>((DWORD_PTR)lua_tointeger(m_L, 1), (float)lua_tonumber(m_L, 2)));
		return 1;
	}
	static int writeDouble(lua_State* m_L)
	{
		lua_pushboolean(m_L, memory.write<double>((DWORD_PTR)lua_tointeger(m_L, 1), (double)lua_tonumber(m_L, 2)));
		return 1;
	}
	// http
	//static int httpGet(lua_State* m_L)
	//{
	//	CHttpClient http;
	//	std::wstring strResponse;
	//	lua_pushinteger(m_L, http.HttpGet(utils::charToWchar(lua_tostring(m_L, 1)).c_str(), utils::charToWchar(lua_tostring(m_L, 2)).c_str(), strResponse));
	//	lua_pushstring(m_L, utils::wcharToChar(strResponse.c_str()).c_str());
	//	return 2;
	//}
	//static int httpPost(lua_State* m_L)
	//{
	//	CHttpClient http;
	//	std::wstring strResponse;
	//	//printf("url->:%ws\n", utils::charToWchar(lua_tostring(m_L, 1)).c_str());
	//	//printf("c->:%ws\n", utils::charToWchar(lua_tostring(m_L, 2)).c_str());
	//	lua_pushinteger(m_L, http.HttpPost(utils::charToWchar(lua_tostring(m_L, 1)).c_str(), utils::charToWchar(lua_tostring(m_L, 2)).c_str(), strResponse));
	//	lua_pushstring(m_L, utils::wcharToChar(strResponse.c_str()).c_str());
	//	return 2;
	//}
private:

};

#endif // !__LUA_H__



