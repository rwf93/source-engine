#include "luastate.h"

#include "tier1/interface.h"
#include "tier2/tier2.h"

#include "luainterface/iluainterface.h"
#include "filesystem.h"

#include "Color.h"

#include "tier0/dbg.h"

#include "tier0/memdbgon.h"

#define GUARD_STATE if(!m_pState) Error("Attempted to call " __FUNCTION__ " without a valid internal lua state being initalized!");

LUA_FUNCTION_STATIC(PrintOverride) 
{
    int nargs = LUA->GetTop();

    for(int i = 1; i <= nargs; i++) 
    {
        LUA->GetGlobal("tostring");
        LUA->Push(i);
        LUA->Call(1, 1);

        ConColorMsg(LUA->GetSide() == LuaStateSide::SERVER ? 
            Color(71, 126, 255, 255) : Color(65, 242, 133, 255), "%s\t", LUA->ToString(-1));
    }

    Msg("\n"); // Finally, add a newline.

    return 0;
}

CLuaState::CLuaState(LuaStateSide side) 
{
    m_pState = 0;
    m_pState = lua_open();
    ((InternalLuaState*)m_pState)->LUA = this;
    m_eSide = side;

    luaL_openlibs(m_pState);

    this->PushFunction(PrintOverride);
    this->SetGlobal("print");

    this->PushInteger(m_eSide == LuaStateSide::CLIENT);
    this->SetGlobal("CLIENT");

    this->PushInteger(m_eSide == LuaStateSide::SERVER);
    this->SetGlobal("SERVER");

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
    if(!g_pFullFileSystem->FileExists("lua/init.lua", "GAME")) return;
    
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

void CLuaState::Push(int index)
{
    GUARD_STATE;
    lua_pushvalue(m_pState, index);
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

void CLuaState::GetGlobal(const char *global)
{
    GUARD_STATE;
    lua_getglobal(m_pState, global);
}

void CLuaState::Call(int nargs, int nresults)
{
    GUARD_STATE;
    lua_call(m_pState, nargs, nresults);
}

int CLuaState::GetTop()
{
    GUARD_STATE;
    return lua_gettop(m_pState);
}

const char *CLuaState::ToString(int index)
{
    GUARD_STATE;
    return lua_tostring(m_pState, index);
}