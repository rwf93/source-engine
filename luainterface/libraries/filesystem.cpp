#include "luacommon.h"
#include "luainterface/iluastate.h"
#include "luainterface/iluainterface.h"

#include "filesystem.h"

#include "tier1/utlbuffer.h"
#include "tier0/memdbgon.h"

LUA_FUNCTION(ReadFile)
{
	CUtlBuffer temp_buffer;

	const char *file = LUA->CheckString(1);
	const char *pathid = LUA->CheckString(2);
	
	g_pFullFileSystem->ReadFile(file, pathid, temp_buffer);

	LUA->PushString(static_cast<const char*>(temp_buffer.String()));

	return 1;
}

LUA_FUNCTION(FileExists)
{
	const char *file = LUA->CheckString(1);
	const char *pathid = LUA->CheckString(2);
	
	LUA->PushBoolean(g_pFullFileSystem->FileExists(file, pathid));

	return 1;
}

LUA_LIBRARY(FileSystem)
{
	LUA->CreateTable();
	{
		LUA_LIBRARY_LFUNCTION(-2, ReadFile);
		LUA_LIBRARY_LFUNCTION(-2, FileExists);
	}
	LUA->SetGlobal("FileSystem");
}