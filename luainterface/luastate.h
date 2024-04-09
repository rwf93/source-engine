#ifndef LUASTATE_H
#define LUASTATE_H
#pragma once

#include <lua.hpp>
#include "luainterface/iluastate.h"

class LuaCommon;

class CLuaState: public ILuaState 
{
public:
    CLuaState(LuaStateSide side);
    ~CLuaState()                            OVERRIDE;

    void Start()                            OVERRIDE;

    LuaStateSide GetSide() OVERRIDE { return m_eSide; }

    void DoString(const char*)              OVERRIDE;
    
    void Push(int index)                    OVERRIDE;
    void PushInteger(int value)             OVERRIDE;
    void PushFunction(CLuaFunctionFn state) OVERRIDE;

    void SetGlobal(const char* global)      OVERRIDE;
    void GetGlobal(const char* global)      OVERRIDE;

    void Call(int nargs, int nresults)      OVERRIDE;

    int GetTop()                            OVERRIDE;

    const char *ToString(int index)         OVERRIDE;
private:
    lua_State *m_pState;
    LuaStateSide m_eSide;
};


#endif