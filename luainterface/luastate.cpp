#include "luastate.h"
#include "luacommon.h"

#include "tier1/interface.h"
#include "tier2/tier2.h"

#include "luainterface/iluainterface.h"
#include "filesystem.h"

#include "tier0/memdbgon.h"

#define GUARD_STATE if(!m_pState) Error("Attempted to call " __FUNCTION__ " without a valid internal lua state being initalized!");

CLuaState::CLuaState(LuaStateSide side)
{
	m_pState = 0;
	m_pState = lua_open();
	((InternalLuaState*)m_pState)->LUA = this; // pretty and ugly
	m_eSide = side;
	m_uIota = -1;

	luaL_openlibs(m_pState);

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

void CLuaState::PushString(const char *string)
{
	GUARD_STATE;
	lua_pushstring(m_pState, string);
}

void CLuaState::PushBoolean(bool boolean)
{
	GUARD_STATE;
	lua_pushboolean(m_pState, boolean);
}

void CLuaState::PushFunction(CLuaFunctionFn fn)
{
	GUARD_STATE;
	lua_pushcfunction(m_pState, (lua_CFunction)fn);
}

void CLuaState::CreateTable()
{
	GUARD_STATE;
	lua_newtable(m_pState);
}

const char *CLuaState::CheckString(int index)
{
	GUARD_STATE;
	return luaL_checkstring(m_pState, index);
}

void CLuaState::SetField(int index, const char *name)
{
	GUARD_STATE;
	lua_setfield(m_pState, index, name);
}

void CLuaState::GetField(int index, const char *name)
{
	GUARD_STATE;
	lua_getfield(m_pState, index, name);
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

void CLuaState::CreateMetaTable(const char *name, UserDataID &id)
{
	GUARD_STATE;
	luaL_newmetatable(m_pState, name);

	if(!m_uMetaMap.HasElement(name)) m_uIota++;
	id = m_uMetaMap[m_uMetaMap.Insert(name, m_uIota)];
}