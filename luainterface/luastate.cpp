#include "luastate.h"
#include "luacommon.h"

#include "tier1/interface.h"
#include "tier2/tier2.h"

#include "luainterface/iluainterface.h"
#include "filesystem.h"

#include "tier0/memdbgon.h"

#define GUARD_STATE 																									\
	do { 																												\
		Assert(m_pState); 																								\
		if(!m_pState) Error("Attempted to call " __FUNCTION__ " without a valid internal lua state being initalized!"); \
	} while(0);

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

	char fullPath[MAX_PATH] = { 0 };
	g_pFullFileSystem->RelativePathToFullPath( "lua/init.lua", "GAME", fullPath, MAX_PATH );

	if(luaL_dofile(m_pState, fullPath) != LUA_OK) { Warning("%s\n", lua_tostring(m_pState, -1)); }
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

void CLuaState::PushMetaTable(UserDataID id)
{
	GUARD_STATE;
	if(!m_uMetaStringTable.HasElement(id)) { Error("Attempted to push invalid metatable typeid!"); return; }
	lua_getfield(m_pState, LUA_REGISTRYINDEX, m_uMetaStringTable[m_uMetaStringTable.Find(id)]);
}

UserdataStruct *CLuaState::CreateUserData(UserDataID id)
{
	UserdataStruct *udata = reinterpret_cast<UserdataStruct*>(lua_newuserdata(m_pState, sizeof(UserdataStruct)));
	if(udata == nullptr) return nullptr;
	udata->id = id;
	return udata;
}

void CLuaState::Pop(int idx)
{
	GUARD_STATE;
	lua_pop(m_pState, idx);
}

void CLuaState::CreateTable()
{
	GUARD_STATE;
	lua_newtable(m_pState);
}

int CLuaState::CreateMetaTable(const char *name, UserDataID &id)
{
	GUARD_STATE;

	if(!m_uMetaIDTable.HasElement(name)) m_uIota++;
	id = m_uMetaIDTable[m_uMetaIDTable.Insert(name, m_uIota)];
	m_uMetaStringTable.Insert(id, name);

	return luaL_newmetatable(m_pState, name);
}

const char *CLuaState::CheckString(int index)
{
	GUARD_STATE;
	return luaL_checkstring(m_pState, index);
}

UserdataStruct *CLuaState::CheckUserData(int idx, UserDataID id)
{
	GUARD_STATE;
	if(!m_uMetaStringTable.HasElement(id)) { Error("Attempted to check invalid metatable typeid!"); return nullptr; }

	auto udata = reinterpret_cast<UserdataStruct*>(luaL_checkudata(m_pState, idx, m_uMetaStringTable[m_uMetaStringTable.Find(id)]));
	if(!udata || udata->id != id) return nullptr;

	return udata;
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

void CLuaState::SetMetaTable(int idx)
{
	GUARD_STATE;
	lua_setmetatable(m_pState, idx);
}

int CLuaState::SetFEnv(int idx)
{
	GUARD_STATE;
	return lua_setfenv(m_pState, idx);
}

void CLuaState::GetFEnv(int idx)
{
	GUARD_STATE;
	lua_getfenv(m_pState, idx);
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
