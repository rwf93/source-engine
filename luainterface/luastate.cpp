#include "luastate.h"

#include "tier1/interface.h"
#include "tier2/tier2.h"

#include "filesystem.h"

#include "tier0/memdbgon.h"

static int PrintOverride(lua_State *L)
{
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i)
    {
        lua_getglobal(L, "tostring");
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        Msg("%s\t", lua_tostring(L, -1));
    }

    Msg("\n");

    return 0;
}

CLuaState::CLuaState() 
{
    state = lua_open();
    luaL_openlibs(state);

    lua_pushcfunction(state, PrintOverride);
    lua_setglobal(state, "print");

    auto opened = g_pFullFileSystem->Open("lua/init.lua", "r", "GAME");
    
    CUtlBuffer buffer;
    g_pFullFileSystem->ReadToBuffer(opened, buffer);
    
    if(luaL_dostring(state, static_cast<const char*>(buffer.String())) != LUA_OK) { Warning("%s\n", lua_tostring(state, -1)); }

    g_pFullFileSystem->Close(opened);
}

CLuaState::~CLuaState() 
{
    lua_close(state);
}