//-----------------------------------------------------------------------------
#include "SoLuaBind.h"
#include "SoLuaErrorHandle.h"
//-----------------------------------------------------------------------------
lua_State* SoLuaBind::ms_L = 0;
SoLuaBind::stElement SoLuaBind::ms_kPushElementList[SoLuaBind_MaxCount_PushElement];
SoLuaBind::stElement SoLuaBind::ms_kPopElementList[SoLuaBind_MaxCount_PopElement];
int SoLuaBind::ms_nBindResultCount = 0;
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
		const char* pszErrorMsg = lua_tostring(ms_L, -1);
		SoLuaErrorHandle::Print("SoLuaBind::ExecuteBind : %s", pszErrorMsg);
		lua_pop(ms_L, 1); //把错误提示信息弹出栈
	}
}
//-----------------------------------------------------------------------------
bool SoLuaBind::FuncBegin()
{
	bool br = true;
	//清零
	ClearTemp();
	//获取参数的个数
	const int nCount = lua_gettop(ms_L);
	//取出所有的参数
	int nPopCount = 0;
	for (int i = -nCount; i < 0; ++i)
	{
		if (CopyResultValue(i, nPopCount) == false)
		{
			br = false;
		}
		++nPopCount;
	}
	return br;
}
//-----------------------------------------------------------------------------
double SoLuaBind::GetDouble(int nIndex, double dfDefault)
{
	double dfValue = dfDefault;
	if (IsValidPopIndex(nIndex))
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
	if (IsValidPopIndex(nIndex))
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
	if (IsValidPopIndex(nIndex))
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
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLuaBind::PushKey : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_double;
	ms_kPushElementList[nPushIndex].dfValue = dfValue;
}
//--------------------------------------------------------------------
void SoLuaBind::PushKey(const char* szValue)
{
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLuaBind::PushKey : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_string;
	ms_kPushElementList[nPushIndex].szValue = szValue; //浅拷贝
}
//--------------------------------------------------------------------
void SoLuaBind::PushKey(bool bValue)
{
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLuaBind::PushKey : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_bool;
	ms_kPushElementList[nPushIndex].dfValue = bValue ? 1.0 : -1.0;
}
//--------------------------------------------------------------------
void SoLuaBind::PushValue(double dfValue)
{
	const int nPopIndex = FindEmptyPopIndex();
	if (nPopIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLuaBind::PushValue : over flow");
		return;
	}
	ms_kPopElementList[nPopIndex].nType = ElementType_double;
	ms_kPopElementList[nPopIndex].dfValue = dfValue;
}
//--------------------------------------------------------------------
void SoLuaBind::PushValue(const char* szValue)
{
	const int nPopIndex = FindEmptyPopIndex();
	if (nPopIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLuaBind::PushValue : over flow");
		return;
	}
	ms_kPopElementList[nPopIndex].nType = ElementType_string;
	ms_kPopElementList[nPopIndex].szValue = szValue; //浅拷贝
}
//--------------------------------------------------------------------
void SoLuaBind::PushValue(bool bValue)
{
	const int nPopIndex = FindEmptyPopIndex();
	if (nPopIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLuaBind::PushValue : over flow");
		return;
	}
	ms_kPopElementList[nPopIndex].nType = ElementType_bool;
	ms_kPopElementList[nPopIndex].dfValue = bValue ? 1.0 : -1.0;
}
//-----------------------------------------------------------------------------
bool SoLuaBind::ArrayEnd(const char* szArrayName, int nElementCount)
{
	if (szArrayName == 0 || szArrayName[0] == 0)
	{
		return false;
	}
	if (nElementCount < 0)
	{
		SoLuaErrorHandle::Print("SoLuaBind::ArrayEnd : invalid element count [%d]", nElementCount);
		nElementCount = 0;
	}
	if (nElementCount > SoLuaBind_MaxCount_PopElement)
	{
		SoLuaErrorHandle::Print("SoLuaBind::ArrayEnd : over flow [%d]", nElementCount);
		nElementCount = SoLuaBind_MaxCount_PopElement;
	}
	lua_createtable(ms_L, nElementCount, 0); //创建一个table，并压入栈
	const int nCount = (nElementCount != 0) ? nElementCount : SoLuaBind_MaxCount_PopElement;
	for (int i = 0; i < nCount; ++i)
	{
		const int theType = ms_kPopElementList[i].nType;
		if (theType == ElementType_Invalid)
		{
			//遍历完毕
			break;
		}
		//
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
bool SoLuaBind::HashEnd(const char* szHashName, int nElementCount)
{
	if (szHashName == 0 || szHashName[0] == 0)
	{
		return false;
	}
	if (nElementCount < 0)
	{
		SoLuaErrorHandle::Print("SoLuaBind::HashEnd : invalid element count [%d]", nElementCount);
		nElementCount = 0;
	}
	if (nElementCount > SoLuaBind_MaxCount_PushElement)
	{
		SoLuaErrorHandle::Print("SoLuaBind::HashEnd : over flow [%d]", nElementCount);
		nElementCount = SoLuaBind_MaxCount_PushElement;
	}
	lua_createtable(ms_L, 0, nElementCount); //创建一个table，并压入栈
	const int nCount = (nElementCount != 0) ? nElementCount : SoLuaBind_MaxCount_PushElement;
	bool bPushFinish = false;
	stElement* pElement[2];
	for (int i = 0; i < nCount; ++i)
	{
		pElement[0] = &(ms_kPushElementList[i]);
		pElement[1] = &(ms_kPopElementList[i]);

		for (int j = 0; j < 2; ++j)
		{
			if (pElement[j]->nType == ElementType_Invalid)
			{
				//遍历完毕
				bPushFinish = true;
			}
		}
		if (bPushFinish == true)
		{
			//遍历完毕
			break;
		}
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
bool SoLuaBind::CopyResultValue(const int nStackIndex, const int nPopElementIndex)
{
	bool br = true;
	const int luatype = lua_type(ms_L, nStackIndex);
	switch (luatype)
	{
	case LUA_TNUMBER:
		{
			double dfV = lua_tonumber(ms_L, nStackIndex);
			ms_kPopElementList[nPopElementIndex].nType = ElementType_double;
			ms_kPopElementList[nPopElementIndex].dfValue = dfV;
			break;
		}
	case LUA_TSTRING:
		{
			const char* szValue = lua_tostring(ms_L, nStackIndex);
			ms_kPopElementList[nPopElementIndex].nType = ElementType_string;
			ms_kPopElementList[nPopElementIndex].szValue = szValue; //浅拷贝
			break;
		}
	case LUA_TBOOLEAN:
		{
			int bV = lua_toboolean(ms_L, nStackIndex);
			ms_kPopElementList[nPopElementIndex].nType = ElementType_bool;
			ms_kPopElementList[nPopElementIndex].dfValue = bV ? 1.0 : -1.0;
			break;
		}
	case LUA_TNIL:
		{
			//脚本逻辑中可能会返回nil，这是正常情况，也要占用一个stElement。
			ms_kPopElementList[nPopElementIndex].nType = ElementType_Invalid;
			break;
		}
	default:
		{
			ms_kPopElementList[nPopElementIndex].nType = ElementType_Invalid;
			br = false;
			//
			const char* szTypeName = lua_typename(ms_L, luatype);
			SoLuaErrorHandle::Print("SoLuaBind::CopyResultValue : invalid output value type[%s]", szTypeName);
			break;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
int SoLuaBind::FindEmptyPushIndex()
{
	int nPushIndex = -1;
	for (int i = 0; i < SoLuaBind_MaxCount_PushElement; ++i)
	{
		if (ms_kPushElementList[i].nType == ElementType_Invalid)
		{
			nPushIndex = i;
			break;
		}
	}
	return nPushIndex;
}
//-----------------------------------------------------------------------------
int SoLuaBind::FindEmptyPopIndex()
{
	int nPopIndex = -1;
	for (int i = 0; i < SoLuaBind_MaxCount_PopElement; ++i)
	{
		if (ms_kPopElementList[i].nType == ElementType_Invalid)
		{
			nPopIndex = i;
			break;
		}
	}
	return nPopIndex;
}
//-----------------------------------------------------------------------------
void SoLuaBind::ClearTemp()
{
	for (int i = 0; i < SoLuaBind_MaxCount_PushElement; ++i)
	{
		ms_kPushElementList[i].nType = ElementType_Invalid;
	}
	for (int i = 0; i < SoLuaBind_MaxCount_PopElement; ++i)
	{
		ms_kPopElementList[i].nType = ElementType_Invalid;
	}
	ms_nBindResultCount = 0;
}
//-----------------------------------------------------------------------------
