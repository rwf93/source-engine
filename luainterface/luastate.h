#ifndef LUASTATE_H
#define LUASTATE_H
#pragma once

#include <lua.hpp>
#include "luainterface/iluastate.h"

class CLuaState: public ILuaState 
{
public:
    CLuaState();
    ~CLuaState() OVERRIDE;

    void RunString(const char*) OVERRIDE;
    void PushFunction(CLuaFunctionFn fn);
private:
    lua_State *m_pState;
};


#endif