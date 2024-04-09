#ifndef LUACOMMON_H
#define LUACOMMON_H
#pragma once

class ILuaState;

#define LUA_LIBRARY(LIBNAME) \
	void LuaLibrary_##LIBNAME(ILuaState *LUA)

#define ELUA_LIBRARY(LIBNAME) \
	extern void LuaLibrary_##LIBNAME(ILuaState *LUA)

#define LUA_LIBRARY_LFUNCTION(INDEX, FUNCNAME) \
	LUA->PushFunction(FUNCNAME); \
	LUA->SetField(INDEX, #FUNCNAME);

#define LUA_LIBRARY_GFUNCTION(FUNCNAME) \
	LUA->PushFunction(FUNCNAME); \
	LUA->SetGlobal(#FUNCNAME);

class LuaCommon
{
public:
	void RegisterCommonLibraries();
};

#endif