#ifndef LUASTATE_H
#define LUASTATE_H
#pragma once

#include <lua.hpp>
#include "luainterface/iluastate.h"

#include <tier1/utlmap.h>
#include <tier1/utlhashtable.h>

class LuaCommon;

class CLuaState: public ILuaState
{
public:
	CLuaState(LuaStateSide side);
	~CLuaState()                            				OVERRIDE;

	void Start()                            				OVERRIDE;
	LuaStateSide GetSide() OVERRIDE { return m_eSide; }
	void DoString(const char*)              				OVERRIDE;

	void Push(int index)                    				OVERRIDE;
	void PushInteger(int value)             				OVERRIDE;
	void PushString(const char *string)     				OVERRIDE;
	void PushBoolean(bool boolean)							OVERRIDE;
	void PushFunction(CLuaFunctionFn state) 				OVERRIDE;

	void Pop(int index)										OVERRIDE;

	void CreateTable()										OVERRIDE;

	const char *CheckString(int index)      				OVERRIDE;

	void SetField(int index, const char* name)      		OVERRIDE;
	void GetField(int index, const char *name)      		OVERRIDE;

	void SetGlobal(const char* global)      				OVERRIDE;
	void GetGlobal(const char* global)      				OVERRIDE;

	void Call(int nargs, int nresults)      				OVERRIDE;

	int GetTop()                            				OVERRIDE;

	const char *ToString(int index)         				OVERRIDE;

	void CreateMetaTable(const char *name, UserDataID &id)	OVERRIDE;
private:
	lua_State *m_pState;
	LuaStateSide m_eSide;

	UserDataID m_uIota; // autoincrementing typeid
	CUtlHashtable<const char*, UserDataID> m_uMetaMap;
};


#endif