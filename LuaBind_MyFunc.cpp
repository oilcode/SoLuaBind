//-----------------------------------------------------------------------------
#include "LuaBind_MyFunc.h"
#include "SoLua.h"
#include "MyFunc.h"
#include "SoLuaBind.h"
//-----------------------------------------------------------------------------
int luaGetWindowIDByName(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SoLuaBind::Return(GetWindowIDByName(SoLuaBind::GetString(0)));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaGetWindowWidth(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SoLuaBind::Return(GetWindowWidth(SoLuaBind::GetInt(0)));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaSetWindowWidth(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SetWindowWidth(SoLuaBind::GetInt(0), SoLuaBind::GetFloat(1));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaGetWindowWidthHeight(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	GetWindowWidthHeight(SoLuaBind::GetInt(0), fWidth, fHeight);
	SoLuaBind::Return(fWidth);
	SoLuaBind::Return(fHeight);
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaSetWindowText(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SetWindowText(SoLuaBind::GetInt(0), SoLuaBind::GetString(1));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaGetWindowText(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SoLuaBind::Return(GetWindowText(SoLuaBind::GetInt(0)));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaWindowHelp_GetWindowPosX(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SoLuaBind::Return(WindowHelp::GetWindowPosX(SoLuaBind::GetInt(0)));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaWindowHelp_SetWindowPosX(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	WindowHelp::SetWindowPosX(SoLuaBind::GetInt(0), SoLuaBind::GetFloat(1));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaWindowHelp_GetWindowPos(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	float fPosX = 0.0f;
	float fPosY = 0.0f;
	WindowHelp::GetWindowPos(SoLuaBind::GetInt(0), fPosX, fPosY);
	SoLuaBind::Return(fPosX);
	SoLuaBind::Return(fPosY);
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaWindowHelp_SetWindowTexture(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	WindowHelp::SetWindowTexture(SoLuaBind::GetInt(0), SoLuaBind::GetString(1));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaWindowHelp_GetWindowTexture(lua_State* pLuaState)
{
	SoLuaBind::FuncBegin();
	SoLuaBind::Return(WindowHelp::GetWindowTexture(SoLuaBind::GetInt(0)));
	return SoLuaBind::FuncEnd();
}
//-----------------------------------------------------------------------------
int luaRegisterMyFunc(lua_State* pLuaState)
{
	SoLuaBind::RegFunction("luaGetWindowIDByName", luaGetWindowIDByName);
	SoLuaBind::RegFunction("luaGetWindowWidth", luaGetWindowWidth);
	SoLuaBind::RegFunction("luaSetWindowWidth", luaSetWindowWidth);
	SoLuaBind::RegFunction("luaGetWindowWidthHeight", luaGetWindowWidthHeight);
	SoLuaBind::RegFunction("luaSetWindowText", luaSetWindowText);
	SoLuaBind::RegFunction("luaGetWindowText", luaGetWindowText);
	//
	luaL_Reg WindowHelp[] = 
	{
		{"GetWindowPosX", luaWindowHelp_GetWindowPosX},
		{"SetWindowPosX", luaWindowHelp_SetWindowPosX},
		{"GetWindowPos", luaWindowHelp_GetWindowPos},
		{"SetWindowTexture", luaWindowHelp_SetWindowTexture},
		{"GetWindowTexture", luaWindowHelp_GetWindowTexture},
		{NULL, NULL},
	};
	SoLuaBind::RegLib("WindowHelp", WindowHelp);


	lua_pushnumber(SoLua::GetLua(), MaxWindowID);
	lua_setglobal(SoLua::GetLua(), "MaxWindowID");

	SoLuaBind::HashBegin();
	SoLuaBind::PushKey("WindowType_Button");
	SoLuaBind::PushValue(WindowType_Button);
	SoLuaBind::PushKey("WindowType_Text");
	SoLuaBind::PushValue(WindowType_Text);
	SoLuaBind::HashEnd("eWindowType");

	//这一类的函数，都要返回0.
	return 0;
}
//-----------------------------------------------------------------------------
