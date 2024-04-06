#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H
#pragma once

#include "luainterface/iluainterface.h"
#include "tier2/tier2.h"

#include <lua.hpp>

class CLuaState: public ILuaState 
{
public:
    CLuaState();
    ~CLuaState() override;
private:
    lua_State *state;
};

class CLuaInterface: public CTier2AppSystem< ILuaInterface > 
{
    typedef CTier2AppSystem< ILuaInterface > BaseClass;
public:
    bool Connect( CreateInterfaceFn factory ) override;

    InitReturnVal_t Init() override;
    void Shutdown() override;

    ILuaState *CreateState() override;
    void DestroyState(ILuaState *state) override;   
};

#endif