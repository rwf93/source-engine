#include "luacommon.h"
#include "luainterface/iluastate.h"

#include "tier0/dbg.h"
#include "Color.h"

#include "tier0/memdbgon.h"

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

LUA_FUNCTION(IsServer)
{
	LUA->PushBoolean(LUA->GetSide() == LuaStateSide::SERVER);
	return 1;
}

LUA_FUNCTION(IsClient)
{
	LUA->PushBoolean(LUA->GetSide() == LuaStateSide::CLIENT);
	return 1;
}

LUA_LIBRARY(Common)
{
	LUA->PushFunction(PrintOverride);
	LUA->SetGlobal("print");

	LUA_LIBRARY_GFUNCTION(IsServer);
	LUA_LIBRARY_GFUNCTION(IsClient);
}