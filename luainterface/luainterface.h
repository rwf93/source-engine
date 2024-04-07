#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H
#pragma once

#include "luainterface/iluainterface.h"
#include "luainterface/iluastate.h"

#include "tier0/platform.h"
#include "tier1/utlmap.h"
#include "tier2/tier2.h"

#include <lua.hpp>

class CLuaInterface: public CTier2AppSystem< ILuaInterface > 
{
    typedef CTier2AppSystem< ILuaInterface > BaseClass;
public:
    bool Connect( CreateInterfaceFn factory ) OVERRIDE;

    InitReturnVal_t Init() OVERRIDE;
    void Shutdown() OVERRIDE;

    ILuaState *CreateState() OVERRIDE;
    void DestroyState(ILuaState *state) OVERRIDE;
};

#endif