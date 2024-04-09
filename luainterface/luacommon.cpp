#include "luacommon.h"
#include "luainterface/iluastate.h"
#include "luainterface/iluainterface.h"

#include "tier2/tier2.h"

#include "tier0/memdbgon.h"

ELUA_LIBRARY(Common);
ELUA_LIBRARY(FileSystem);

void LuaCommon::RegisterCommonLibraries()
{
    g_pLuaInterface->RegisterLib(LuaStateSide::CLIENT, LuaLibrary_Common);
    g_pLuaInterface->RegisterLib(LuaStateSide::SERVER, LuaLibrary_Common);
    g_pLuaInterface->RegisterLib(LuaStateSide::CLIENT, LuaLibrary_FileSystem);
    g_pLuaInterface->RegisterLib(LuaStateSide::SERVER, LuaLibrary_FileSystem);
}