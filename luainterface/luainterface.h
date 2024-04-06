#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H
#pragma once

#include "luainterface/iluainterface.h"
#include "luainterface/iluastate.h"

#include "tier2/tier2.h"

#include <lua.hpp>

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