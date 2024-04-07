#include "luastate.h"

#include "tier1/interface.h"
#include "tier2/tier2.h"

#include "filesystem.h"

#include "tier0/memdbgon.h"

#define GUARD_STATE if(!m_pState) Error("Attempted to call " __FUNCTION__ " without a valid internal lua state being initalized!");

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
    m_pState = 0;
    m_pState = lua_open();
    ((lua_StateUserdata*)m_pState)->state_userdata = reinterpret_cast<void*>(this);

    luaL_openlibs(m_pState);

    lua_pushcfunction(m_pState, PrintOverride);
    lua_setglobal(m_pState, "print");

    auto opened = g_pFullFileSystem->Open("lua/init.lua", "r", "GAME");
    
    CUtlBuffer buffer;
    g_pFullFileSystem->ReadToBuffer(opened, buffer);
    
    if(luaL_dostring(m_pState, static_cast<const char*>(buffer.String())) != LUA_OK) { Warning("%s\n", lua_tostring(m_pState, -1)); }

    g_pFullFileSystem->Close(opened);
}

CLuaState::~CLuaState() 
{
    lua_close(m_pState);
    m_pState = 0;
}

void CLuaState::RunString(const char* string) 
{
    GUARD_STATE;
    if(luaL_dostring(m_pState, string) != LUA_OK) { Warning("%s\n", lua_tostring(m_pState, -1)); }
}

void CLuaState::PushFunction(CLuaFunctionFn fn) 
{
}