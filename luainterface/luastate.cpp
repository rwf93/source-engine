#include "luastate.h"

#include "tier1/interface.h"
#include "tier2/tier2.h"

#include "luainterface/iluainterface.h"

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

CLuaState::CLuaState(LuaStateSide side) 
{
    m_pState = 0;
    m_pState = lua_open();
    ((InternalLuaState*)m_pState)->luastate = this;
    
    luaL_openlibs(m_pState);

    lua_pushcfunction(m_pState, PrintOverride);
    lua_setglobal(m_pState, "print");

    m_eSide = side;

    // Register Sided Lua Libraries
    g_pLuaInterface->SetupLuaLibraries(m_eSide, this);
}

CLuaState::~CLuaState() 
{
    lua_close(m_pState);
    m_pState = 0;
}

void CLuaState::Start()
{
    auto opened = g_pFullFileSystem->Open("lua/init.lua", "r", "GAME");
    
    CUtlBuffer buffer;
    g_pFullFileSystem->ReadToBuffer(opened, buffer);
    
    if(luaL_dostring(m_pState, static_cast<const char*>(buffer.String())) != LUA_OK) { Warning("%s\n", lua_tostring(m_pState, -1)); }

    g_pFullFileSystem->Close(opened);
}

void CLuaState::DoString(const char* string) 
{
    GUARD_STATE;
    if(luaL_dostring(m_pState, string) != LUA_OK) { Warning("%s\n", lua_tostring(m_pState, -1)); }
}

void CLuaState::PushInteger(int value)
{
    GUARD_STATE;
    lua_pushinteger(m_pState, value);
}

void CLuaState::PushFunction(CLuaFunctionFn fn) 
{
    GUARD_STATE;
    lua_pushcfunction(m_pState, (lua_CFunction)fn);
}

void CLuaState::SetGlobal(const char *global)
{
    GUARD_STATE;
    lua_setglobal(m_pState, global);
}