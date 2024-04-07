#ifndef LUASTATE_H
#define LUASTATE_H
#pragma once

#include <lua.hpp>
#include "luainterface/iluastate.h"

class CLuaState: public ILuaState 
{
public:
    CLuaState(LuaStateSide side);
    ~CLuaState() OVERRIDE;

    void Start() OVERRIDE;

    void DoString(const char*) OVERRIDE;
    void PushFunction(CLuaFunctionFn fn) OVERRIDE;
    void PushInteger(int) OVERRIDE;
    
    void SetGlobal(const char*) OVERRIDE;
private:
    lua_State *m_pState;
    LuaStateSide m_eSide;
};


#endif