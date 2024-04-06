#ifndef ILUASTATE_H
#define ILUASTATE_H
#pragma once

#include "tier1/interface.h"

abstract_class ILuaState 
{
public:
    virtual ~ILuaState() {}
    virtual void RunString(const char*) = 0;
};

#endif