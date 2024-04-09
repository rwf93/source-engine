#ifndef LUACOMMON_H
#define LUACOMMON_H
#pragma once

class ILuaState;

#define LUA_LIBRARY(LIBNAME) \
    void LuaLibrary_##LIBNAME(ILuaState *LUA)

#define ELUA_LIBRARY(LIBNAME) \
    extern void LuaLibrary_##LIBNAME(ILuaState *LUA)
 
class LuaCommon
{
public:
    void RegisterCommonLibraries();
};

#endif