//-----------------------------------------------------------------------------
#ifndef _LuaBind_MyFunc_h_
#define _LuaBind_MyFunc_h_
//-----------------------------------------------------------------------------
struct lua_State;
//-----------------------------------------------------------------------------
int luaGetWindowIDByName(lua_State* pLuaState);
int luaGetWindowWidth(lua_State* pLuaState);
int luaSetWindowWidth(lua_State* pLuaState);
int luaGetWindowWidthHeight(lua_State* pLuaState);
int luaSetWindowText(lua_State* pLuaState);
int luaGetWindowText(lua_State* pLuaState);
//
int luaWindowHelp_GetWindowPosX(lua_State* pLuaState);
int luaWindowHelp_SetWindowPosX(lua_State* pLuaState);
int luaWindowHelp_GetWindowPos(lua_State* pLuaState);
int luaWindowHelp_SetWindowTexture(lua_State* pLuaState);
int luaWindowHelp_GetWindowTexture(lua_State* pLuaState);
//
int luaRegisterMyFunc(lua_State* pLuaState);
//-----------------------------------------------------------------------------
#endif //_LuaBind_MyFunc_h_
//-----------------------------------------------------------------------------
