#ifndef ILUASTATE_H
#define ILUASTATE_H
#pragma once

#include "tier1/interface.h"

class ILuaState;
typedef int(*CLuaFunctionFn)(ILuaState*);

struct lua_StateUserdata
{
#ifdef PLATFORM_64BITS
    char base[96]; // Generic LuaState Data
#else
    char base[48]
#endif

    void *state_userdata;
};

abstract_class ILuaState 
{
public:
    virtual ~ILuaState() {}
    virtual void RunString(const char*) = 0;
    virtual void PushFunction(CLuaFunctionFn state) = 0;
};

#endif