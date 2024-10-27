#include "luacommon.h"
#include "luainterface/iluastate.h"
#include "luainterface/iluainterface.h"

#include "filesystem.h"

#include "tier1/utlbuffer.h"
#include "tier1/utlhashtable.h"
#include "tier0/memdbgon.h"

static const char *FileHandleUserdata = "FileHandle";
static UserDataID FileHandleUserdataID = 0;

UserdataStruct *PushFileHandle(ILuaState *LUA, FileHandle_t handle)
{
	auto userdata = LUA->CreateUserData(FileHandleUserdataID);
	userdata->opaque = reinterpret_cast<void*>(handle);

	LUA->PushMetaTable(FileHandleUserdataID);
	LUA->SetMetaTable(-2);

	LUA->CreateTable();
	LUA->SetFEnv(-2);

	return userdata;
}

FileHandle_t CheckFileHandle(ILuaState *LUA, int idx)
{
	auto udata = LUA->CheckUserData(idx, FileHandleUserdataID);
	if(!udata) return nullptr;
	return reinterpret_cast<FileHandle_t>(udata->opaque);
}

LUA_FUNCTION_STATIC(Open)
{
	const char *path = LUA->CheckString(1);
	const char *mode = LUA->CheckString(2);
	const char *pathid = LUA->CheckString(3);

	auto filehandle = g_pFullFileSystem->Open(path, mode, pathid);
	PushFileHandle(LUA, filehandle);

	return 1;
}

LUA_FUNCTION_STATIC(__gc)
{
	auto filehandle = CheckFileHandle(LUA, 1);
	if(!filehandle) return 0;

	g_pFullFileSystem->Close(filehandle);

	return 0;
}

LUA_FUNCTION_STATIC(TestFileHandle)
{
	auto filehandle = CheckFileHandle(LUA, 1);
	Msg("Cool filehandle at %p\n", filehandle);
	return 0;
}

LUA_FUNCTION_STATIC(Close)
{
	__gc_Impl(LUA); // that's evil and rank
	return 0;
}

LUA_FUNCTION_STATIC(ReadFile)
{
	CUtlBuffer temp_buffer;

	const char *file = LUA->CheckString(1);
	const char *pathid = LUA->CheckString(2);

	g_pFullFileSystem->ReadFile(file, pathid, temp_buffer);

	LUA->PushString(static_cast<const char*>(temp_buffer.String()));

	return 1;
}

LUA_FUNCTION_STATIC(FileExists)
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
		LUA_LIBRARY_LFUNCTION(-2, Open);
		LUA_LIBRARY_LFUNCTION(-2, ReadFile);
		LUA_LIBRARY_LFUNCTION(-2, FileExists);
	}
	LUA->SetGlobal("FileSystem");

	LUA->CreateMetaTable(FileHandleUserdata, FileHandleUserdataID);
	{
		LUA->PushInteger(-1);
		LUA->SetField(-2, "__index");

		LUA->PushInteger(-1);
		LUA->SetField(-2, "__newindex");

		LUA_LIBRARY_LFUNCTION(-2, __gc);

		LUA_LIBRARY_LFUNCTION(-2, TestFileHandle);
		LUA_LIBRARY_LFUNCTION(-2, Close);
	}
	LUA->Pop(-1);
}