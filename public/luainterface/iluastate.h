#ifndef ILUASTATE_H
#define ILUASTATE_H
#pragma once

#include "tier1/interface.h"

// Extreme hack to use ILuaStates instead of rawdogging the lua_State
struct lua_StateUserdata
{
#ifdef PLATFORM_64BITS
    char base[96]; // Generic LuaState Data
#else
    char base[48]; // Generic LuaState Data
#endif

    void *state_userdata;
};

typedef int(*CLuaFunctionFn)(lua_StateUserdata*);

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

    virtual void DoString(const char*) = 0;
    virtual void PushFunction(CLuaFunctionFn state) = 0;
    virtual void PushInteger(int value) = 0;
    virtual void SetGlobal(const char*) = 0;
};

#endif