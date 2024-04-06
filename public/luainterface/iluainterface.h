#ifndef ILUAINTERFACE_H
#define ILUAINTERFACE_H
#pragma once

#include "tier1/interface.h"
#include "appframework/IAppSystem.h"

class ILuaState;

#include <limits.h>

#define BASELUA_INTERFACE_VERSION		"VBaseLuaInterface01"

abstract_class ILuaInterface: public IAppSystem
{
public:
    virtual ILuaState *CreateState() = 0;
    virtual void DestroyState(ILuaState *state) = 0;
};

#endif