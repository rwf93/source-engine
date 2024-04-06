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

ILuaState *CLuaInterface::CreateState()
{
    return (ILuaState*)new CLuaState();
}

void CLuaInterface::DestroyState(ILuaState *state)
{
    delete state;
}