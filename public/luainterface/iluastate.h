#ifndef ILUASTATE_H
#define ILUASTATE_H
#pragma once

#include "tier1/interface.h"

// Extreme hack to use ILuaStates instead of rawdogging the lua_State
class ILuaState;
struct InternalLuaState
{
#ifdef PLATFORM_64BITS
    char base[96]; // Generic LuaState Data
#else
    char base[48]; // Generic LuaState Data
#endif
    ILuaState *LUA;
};

typedef int(*CLuaFunctionFn)(InternalLuaState*);

#define LUA_FUNCTION(FUNCTION) \
    int FUNCTION##_Impl(ILuaState *LUA); \
    int FUNCTION(InternalLuaState *L) { return FUNCTION##_Impl(L->LUA);  } \
    int FUNCTION##_Impl(ILuaState *LUA)

#define LUA_FUNCTION_STATIC(FUNCTION) \
    int FUNCTION##_Impl(ILuaState *LUA); \
    static int FUNCTION(InternalLuaState *L) { return FUNCTION##_Impl(L->LUA);  } \
    int FUNCTION##_Impl(ILuaState *LUA)

enum LuaStateSide
{
    SERVER,
    CLIENT
};

abstract_class ILuaState 
{
public:
    virtual ~ILuaState() {}

    virtual void Start() = 0;

    virtual LuaStateSide GetSide() = 0;

    virtual void DoString(const char* code) = 0;

    virtual void Push(int index) = 0;
    virtual void PushInteger(int value) = 0;
    virtual void PushFunction(CLuaFunctionFn state) = 0;

    virtual void SetGlobal(const char* global) = 0;
    virtual void GetGlobal(const char* global) = 0;

    virtual void Call(int nargs, int nresults) = 0;

    virtual int GetTop() = 0;

    virtual const char *ToString(int index) = 0;
};

#endif