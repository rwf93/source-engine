#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H
#pragma once

#include "luainterface/iluainterface.h"
#include "luainterface/iluastate.h"

#include "tier0/platform.h"
#include "tier1/utlmap.h"
#include "tier2/tier2.h"

#include <lua.hpp>

#include "luacommon.h"

struct CRegisteredLib
{
	LuaStateSide side;
	CLuaLibFn fn;
};

class CLuaInterface: public CTier2AppSystem< ILuaInterface > 
{
	typedef CTier2AppSystem< ILuaInterface > BaseClass;
public:
	bool Connect( CreateInterfaceFn factory ) OVERRIDE;

	InitReturnVal_t Init() OVERRIDE;
	void Shutdown() OVERRIDE;

	ILuaState *CreateState(LuaStateSide side) OVERRIDE;
	void DestroyState(ILuaState *state) OVERRIDE;

	void RegisterLib(LuaStateSide side, CLuaLibFn fn) OVERRIDE;
	void SetupLuaLibraries(LuaStateSide side, ILuaState* state) OVERRIDE;
private:
	CUtlVector<CRegisteredLib> m_vRegistedLibs;
	LuaCommon __internal_common_library_dont_touch;
};

#endif