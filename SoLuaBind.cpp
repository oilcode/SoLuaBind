//-----------------------------------------------------------------------------
#include "SoLuaBind.h"
#include "SoLuaErrorHandle.h"
//-----------------------------------------------------------------------------
lua_State* SoLuaBind::ms_L = 0;
SoLuaBind::stElement SoLuaBind::ms_kPushElementList[SoLuaBind_MaxCount];
SoLuaBind::stElement SoLuaBind::ms_kPopElementList[SoLuaBind_MaxCount];
int SoLuaBind::ms_nPushSize = 0;
int SoLuaBind::ms_nPopSize = 0;
int SoLuaBind::ms_nBindResultCount = 0;
bool SoLuaBind::ms_bHandleError = false;
//-----------------------------------------------------------------------------
void SoLuaBind::InitLuaBind(lua_State* L)
{
	ms_L = L;
}
//-----------------------------------------------------------------------------
void SoLuaBind::ReleaseLuaBind()
{
	ms_L = 0;
}
//-----------------------------------------------------------------------------
void SoLuaBind::ExecuteBind(lua_CFunction pBindFunc)
{
	if (ms_L == 0)
	{
		return;
	}

	lua_pushcfunction(ms_L, pBindFunc);
	if (lua_pcall(ms_L, 0, 0, 0) == 0)
	{
		//执行成功，什么也不做。
	}
	else
	{
		if (ms_bHandleError)
		{
			const char* pszErrorMsg = lua_tostring(ms_L, -1);
			SoLuaErrorHandle::Print("SoLuaBind::ExecuteBind : %s", pszErrorMsg);
		}
		lua_pop(ms_L, 1); //把错误提示信息弹出栈
	}
}
//-----------------------------------------------------------------------------
bool SoLuaBind::FuncBegin()
{
	//清零
	ms_nPopSize = 0;
	ms_nBindResultCount = 0;

	//获取参数的个数
	const int nCount = lua_gettop(ms_L);
	if (nCount > SoLuaBind_MaxCount)
	{
		if (ms_bHandleError)
		{
			SoLuaErrorHandle::Print("SoLuaBind::FuncBegin : too many params");
		}
		return false;
	}

	bool br = true;
	//取出所有的参数
	for (int i = -nCount; i < 0; ++i)
	{
		if (CopyResultValue(i, &(ms_kPopElementList[ms_nPopSize])) == false)
		{
			br = false;
		}
		++ms_nPopSize;
	}
	return br;
}
//-----------------------------------------------------------------------------
double SoLuaBind::GetDouble(int nIndex, double dfDefault)
{
	double dfValue = dfDefault;
	if (nIndex >= 0 && nIndex < ms_nPopSize)
	{
		if (ms_kPopElementList[nIndex].nType == ElementType_double)
		{
			dfValue = ms_kPopElementList[nIndex].dfValue;
		}
	}
	return dfValue;
}
//-----------------------------------------------------------------------------
const char* SoLuaBind::GetString(int nIndex, const char* szDefault)
{
	const char* szValue = szDefault;
	if (nIndex >= 0 && nIndex < ms_nPopSize)
	{
		if (ms_kPopElementList[nIndex].nType == ElementType_string)
		{
			szValue = ms_kPopElementList[nIndex].szValue;
		}
	}
	return szValue;
}
//-----------------------------------------------------------------------------
bool SoLuaBind::GetBool(int nIndex, bool bDefault)
{
	bool bValue = bDefault;
	if (nIndex >= 0 && nIndex < ms_nPopSize)
	{
		if (ms_kPopElementList[nIndex].nType == ElementType_bool)
		{
			bValue = (ms_kPopElementList[nIndex].dfValue > 0.0);
		}
	}
	return bValue;
}
//--------------------------------------------------------------------
void SoLuaBind::PushKey(double dfValue)
{
	if (ms_nPushSize > SoLuaBind_MaxCount)
	{
		return;
	}
	ms_kPushElementList[ms_nPushSize].nType = ElementType_double;
	ms_kPushElementList[ms_nPushSize].dfValue = dfValue;
	++ms_nPushSize;
}
//--------------------------------------------------------------------
void SoLuaBind::PushKey(const char* szValue)
{
	if (ms_nPushSize > SoLuaBind_MaxCount)
	{
		return;
	}
	ms_kPushElementList[ms_nPushSize].nType = ElementType_string;
	ms_kPushElementList[ms_nPushSize].szValue = szValue;
	++ms_nPushSize;
}
//--------------------------------------------------------------------
void SoLuaBind::PushKey(bool bValue)
{
	if (ms_nPushSize > SoLuaBind_MaxCount)
	{
		return;
	}
	ms_kPushElementList[ms_nPushSize].nType = ElementType_bool;
	ms_kPushElementList[ms_nPushSize].dfValue = bValue ? 1.0 : -1.0;
	++ms_nPushSize;
}
//--------------------------------------------------------------------
void SoLuaBind::PushValue(double dfValue)
{
	if (ms_nPopSize > SoLuaBind_MaxCount)
	{
		return;
	}
	ms_kPopElementList[ms_nPopSize].nType = ElementType_double;
	ms_kPopElementList[ms_nPopSize].dfValue = dfValue;
	++ms_nPopSize;
}
//--------------------------------------------------------------------
void SoLuaBind::PushValue(const char* szValue)
{
	if (ms_nPopSize > SoLuaBind_MaxCount)
	{
		return;
	}
	ms_kPopElementList[ms_nPopSize].nType = ElementType_string;
	ms_kPopElementList[ms_nPopSize].szValue = szValue;
	++ms_nPopSize;
}
//--------------------------------------------------------------------
void SoLuaBind::PushValue(bool bValue)
{
	if (ms_nPopSize > SoLuaBind_MaxCount)
	{
		return;
	}
	ms_kPopElementList[ms_nPopSize].nType = ElementType_bool;
	ms_kPopElementList[ms_nPopSize].dfValue = bValue ? 1.0 : -1.0;
	++ms_nPopSize;
}
//-----------------------------------------------------------------------------
bool SoLuaBind::ArrayEnd(const char* szArrayName)
{
	if (szArrayName == 0 || szArrayName[0] == 0)
	{
		return false;
	}

	lua_createtable(ms_L, ms_nPopSize, 0); //创建一个table，并压入栈
	for (int i = 0; i < ms_nPopSize; ++i)
	{
		const int theType = ms_kPopElementList[i].nType;
		switch (theType)
		{
		case ElementType_double:
			{
				lua_pushnumber(ms_L, ms_kPopElementList[i].dfValue); //压入栈
				break;
			}
		case ElementType_string:
			{
				lua_pushstring(ms_L, ms_kPopElementList[i].szValue); //压入栈
				break;
			}
		case ElementType_bool:
			{
				int b = ms_kPopElementList[i].dfValue > 0.0 ? 1 : 0;
				lua_pushboolean(ms_L, b); //压入栈
				break;
			}
		default:
			{
				//数组中有个元素是nil
				lua_pushnil(ms_L); //压入nil
				break;
			}
		} //switch
		//数组下标是 i+1
		lua_rawseti(ms_L, -2, i+1); //以(i+1)为key，以栈顶元素为value，插入到位于(-2)位置的table中，然后把栈顶元素弹出，使得table位于栈顶
	}
	lua_setglobal(ms_L, szArrayName); //将栈顶的table设置到全局变量szArrayName中，并弹出栈顶的table
	return true;
}
//-----------------------------------------------------------------------------
bool SoLuaBind::HashEnd(const char* szHashName)
{
	if (szHashName == 0 || szHashName[0] == 0)
	{
		return false;
	}

	lua_createtable(ms_L, 0, ms_nPushSize); //创建一个table，并压入栈
	bool bPushFinish = false;
	stElement* pElement[2];
	for (int i = 0; i < ms_nPushSize; ++i)
	{
		pElement[0] = &(ms_kPushElementList[i]);
		pElement[1] = &(ms_kPopElementList[i]);
		//
		for (int k = 0; k < 2; ++k)
		{
			switch (pElement[k]->nType)
			{
			case ElementType_double:
				{
					lua_pushnumber(ms_L, pElement[k]->dfValue); //压入栈
					break;
				}
			case ElementType_string:
				{
					lua_pushstring(ms_L, pElement[k]->szValue); //压入栈
					break;
				}
			case ElementType_bool:
				{
					int b = pElement[k]->dfValue > 0.0 ? 1 : 0;
					lua_pushboolean(ms_L, b); //压入栈
					break;
				}
			default:
				{
					//数组中有个元素是nil
					lua_pushnil(ms_L); //压入nil
					break;
				}
			} //switch
		}
		lua_rawset(ms_L, -3); //以栈中-2位置的元素为key，以栈顶元素为value，插入到位于(-3)位置的table中，然后把-2和栈顶元素弹出，使得table位于栈顶
	}
	lua_setglobal(ms_L, szHashName); //将栈顶的table设置到全局变量szHashName中，并弹出栈顶的table
	return true;
}
//-----------------------------------------------------------------------------
bool SoLuaBind::CopyResultValue(const int nStackIndex, stElement* pElement)
{
	bool br = true;
	const int luatype = lua_type(ms_L, nStackIndex);
	switch (luatype)
	{
	case LUA_TNUMBER:
		{
			pElement->nType = ElementType_double;
			pElement->dfValue = lua_tonumber(ms_L, nStackIndex);
			break;
		}
	case LUA_TSTRING:
		{
			pElement->nType = ElementType_string;
			pElement->szValue = lua_tostring(ms_L, nStackIndex);
			break;
		}
	case LUA_TBOOLEAN:
		{
			pElement->nType = ElementType_bool;
			const int bV = lua_toboolean(ms_L, nStackIndex);
			pElement->dfValue = bV ? 1.0 : -1.0;
			break;
		}
	case LUA_TNIL:
		{
			//脚本逻辑中可能会返回nil，这是正常情况，也要占用一个stElement。
			pElement->nType = ElementType_Invalid;
			break;
		}
	default:
		{
			pElement->nType = ElementType_Invalid;
			br = false;
			//
			if (ms_bHandleError)
			{
				const char* szTypeName = lua_typename(ms_L, luatype);
				SoLuaErrorHandle::Print("SoLuaBind::CopyResultValue : invalid output value type[%s]", szTypeName);
			}
			break;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
