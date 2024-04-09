#ifndef ILUAINTERFACE_H
#define ILUAINTERFACE_H
#pragma once

#include "tier1/interface.h"
#include "appframework/IAppSystem.h"

class ILuaState;
enum LuaStateSide;

typedef void(*CLuaLibFn)(ILuaState*);

#include <limits.h>

#define BASELUA_INTERFACE_VERSION		"VBaseLuaInterface01"

abstract_class ILuaInterface: public IAppSystem
{
public:
	virtual ILuaState *CreateState(LuaStateSide side) = 0;
	virtual void DestroyState(ILuaState *state) = 0;
	virtual void RegisterLib(LuaStateSide side, CLuaLibFn fn) = 0; // Internal, not recommended to use
	virtual void SetupLuaLibraries(LuaStateSide side, ILuaState *state) = 0; // Internal, not recommended to use
};

#endif