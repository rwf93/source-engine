#ifndef LUASTATE_H
#define LUASTATE_H
#pragma once

#include <lua.hpp>
#include "luainterface/iluastate.h"

class CLuaState: public ILuaState 
{
public:
    CLuaState();
    ~CLuaState() override;
private:
    lua_State *state;
};


#endif