#include "luainterface.h"
#include "luastate.h"

#include "tier1/interface.h"
#include "tier0/memdbgon.h"

static CLuaInterface g_LuaInterface;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CLuaInterface, ILuaInterface, BASELUA_INTERFACE_VERSION, g_LuaInterface );

bool CLuaInterface::Connect( CreateInterfaceFn factory )
{
    if(!BaseClass::Connect(factory))
        return false;

    return true;
}

InitReturnVal_t CLuaInterface::Init()
{
    InitReturnVal_t nRetVal = BaseClass::Init();
	if ( nRetVal != INIT_OK )
		return nRetVal;

    return INIT_OK;
}

void CLuaInterface::Shutdown()
{
    BaseClass::Shutdown();
}

ILuaState *CLuaInterface::CreateState(LuaStateSide side)
{
    return (ILuaState*)new CLuaState(side);
}

void CLuaInterface::DestroyState(ILuaState *state)
{
    delete state;
}

void CLuaInterface::RegisterLib(LuaStateSide side, CLuaLibFn fn)
{
    m_vRegistedLibs.AddToTail({side, fn});
}

void CLuaInterface::SetupLuaLibraries(LuaStateSide side, ILuaState *state)
{
    for(auto &lib: m_vRegistedLibs) 
    {
        if(lib.side == side)
            lib.fn(state);
    }
}