//-----------------------------------------------------------------------------
//1，用户可以压入SoLua_MaxCount_PushElement个参数（这些参数包括table名字，函数名字和函数参数），
//   然后执行CallEnd()调用lua脚本函数，得到的返回值都暂时存储在本类中，用户可以调用GetXX()得到返回值。
//   必须严格按照这样的压入顺序：先压入table名字，再压入函数名字，然后压入函数参数。
//   参数和返回值都必须是简单类型的变量。
//2，用户可以指定table和key，得到键值对中的value。
//   用户可以压入SoLua_MaxCount_PushElement个key（包括table名字和key名字），
//   然后执行FieldEnd()，得到的value都暂时存储在本类中，用户可以调用GetXX()得到value。
//   必须严格按照这样的压入顺序：先压入table名字，再压入key名字。
//   参数和返回值都必须是简单类型的变量。
//3，目前，只能获取键值对中的value，不能设置键值对中的value。
//4，压入栈时，使用push系列的函数，不需要指定序号，依次逐个压入栈。
//   取出栈内元素时，使用get系列函数，需要指定序号，这样做的用意是，避免一个语法上的陷阱。
//   例如，依次取出栈内元素后，第一个元素是theWindowID，第二个元素是theWindowWidth，
//   用户希望的效果是 SetWindowWidth(theWindowID, theWindowWidth) ，
//   如果不指定序号，执行 SetWindowWidth(get(), get()) 后，用户得到的效果是
//   SetWindowWidth(theWindowWidth, theWindowID)
//   正确的写法是 SetWindowWidth(get(0), get(1))
//5，压入的值不能是NULL(nil)，因为NULL的值是0，Push(0)不能区分出是压入数字0还是压入NULL。
//-----------------------------------------------------------------------------
#include "SoLua.h"
#include "SoLuaErrorHandle.h"
#include <cstdio>
#include <string>
//-----------------------------------------------------------------------------
#define SoLuaResultOK 1
#define SoLuaResultError 0
//-----------------------------------------------------------------------------
lua_State* SoLua::ms_L = 0;
SoLua::stElement SoLua::ms_kPushElementList[SoLua_MaxCount_PushElement];
SoLua::stElement SoLua::ms_kPopElementList[SoLua_MaxCount_PopElement];
char SoLua::ms_szStringValueList[SoLua_MaxStringCount][SoLua_MaxStringSize];
//-----------------------------------------------------------------------------
bool SoLua::InitLua()
{
	bool br = true;
	if (ms_L == 0)
	{
		ms_L = lua_open();
		if (ms_L)
		{
			lua_pushcfunction(ms_L, &PMode_OpenLibs);
			if (lua_pcall(ms_L, 0, 0, 0) == 0)
			{
				br = true;
			}
			else
			{
				HandleLuaAPIError();
				br = false;
			}
		}
		else
		{
			SoLuaErrorHandle::Print("SoLua::InitLua : lua_open() fail!");
			br = false;
		}
	}
	//
	if (br == false)
	{
		ReleaseLua();
	}
	return br;
}
//-----------------------------------------------------------------------------
void SoLua::ReleaseLua()
{
	if (ms_L)
	{
		lua_close(ms_L);
		ms_L = 0;
	}
}
//-----------------------------------------------------------------------------
bool SoLua::ExecuteFile(const char* pszLuaFile)
{
	if (ms_L == 0)
	{
		return false;
	}
	if (pszLuaFile == 0 || pszLuaFile[0] == 0)
	{
		return false;
	}
	//从lua文件中获取文件内容。
	//可以是二进制内容也可以是文本内容。
	char* pFileBuff = 0;
	int nFileSize = 0;
	if (LoadFileToBuff(pszLuaFile, &pFileBuff, nFileSize) == false)
	{
		return false;
	}
	//编译并执行。
	bool br = ExecuteTrunk(pFileBuff, nFileSize);
	//释放缓存。
	if (pFileBuff)
	{
		free(pFileBuff);
	}
	return br;
}
//--------------------------------------------------------------------
bool SoLua::ExecuteTrunk(const char* pBuff, int nBuffSize)
{
	if (ms_L == 0)
	{
		return false;
	}
	if (pBuff == 0)
	{
		return false;
	}
	if (nBuffSize <= 0)
	{
		nBuffSize = (int)strlen(pBuff);
	}

	int nResult = SoLuaResultError;
	lua_pushcfunction(ms_L, &PMode_ExecuteTrunk);
	lua_pushlightuserdata(ms_L, (void*)pBuff);
	lua_pushinteger(ms_L, nBuffSize);
	if (lua_pcall(ms_L, 2, 1, 0) == 0)
	{
		nResult = lua_tointeger(ms_L, -1); //取出返回值
		lua_pop(ms_L, 1); //把返回值弹出栈
	}
	else
	{
		HandleLuaAPIError();
	}

	return (nResult == SoLuaResultOK);
}
//--------------------------------------------------------------------
void SoLua::DumpStack()
{
	if (ms_L == 0)
	{
		return;
	}

	lua_State* L = ms_L;
	const int count = lua_gettop(L);
	SoLuaErrorHandle::Print("SoLua::DumpStack : begin; element count[%d]", count);
	//
	for (int i = 1; i <= count; ++i)
	{
		const int type = lua_type(L, i);
		switch (type)
		{
		case LUA_TSTRING:
			{
				const char* szString = lua_tostring(L, i);
				SoLuaErrorHandle::Print("SoLua::DumpStack : [%d][string][%s]", i, szString);
				break;
			}
		case LUA_TBOOLEAN:
			{
				const char* szBool = lua_toboolean(L, i) ? "true" : "false";
				SoLuaErrorHandle::Print("SoLua::DumpStack : [%d][bool][%s]", i, szBool);
				break;
			}
		case LUA_TNUMBER:
			{
				double dfNumber = lua_tonumber(L, i);
				SoLuaErrorHandle::Print("SoLua::DumpStack : [%d][number][%f]", i, dfNumber);
				break;
			}
		default:
			{
				const char* szTypeName = lua_typename(L, type);
				SoLuaErrorHandle::Print("SoLua::DumpStack : [%d][%s]", i, szTypeName);
				break;
			}
		}
	}
	SoLuaErrorHandle::Print("SoLua::DumpStack : end");
}
//--------------------------------------------------------------------
void SoLua::PushTable(const char* szTableName)
{
	if (szTableName == 0 || szTableName[0] == 0)
	{
		SoLuaErrorHandle::Print("SoLua::PushTable : szTableName is invalid");
		return;
	}
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLua::PushTable : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_string_TableName;
	ms_kPushElementList[nPushIndex].szValue = szTableName;
}
//--------------------------------------------------------------------
void SoLua::PushFunc(const char* szFuncName)
{
	if (szFuncName == 0 || szFuncName[0] == 0)
	{
		SoLuaErrorHandle::Print("SoLua::PushFunc : szFuncName is invalid");
		return;
	}
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLua::PushFunc : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_string_FuncName;
	ms_kPushElementList[nPushIndex].szValue = szFuncName;
}
//--------------------------------------------------------------------
void SoLua::Push(const char* szValue)
{
	if (szValue == 0)
	{
		SoLuaErrorHandle::Print("SoLua::Push : szValue is invalid");
		return;
	}
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLua::Push : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_string;
	ms_kPushElementList[nPushIndex].szValue = szValue;
}
//--------------------------------------------------------------------
void SoLua::Push(double dfValue)
{
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLua::Push : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_double;
	ms_kPushElementList[nPushIndex].dfValue = dfValue;
}
//--------------------------------------------------------------------
void SoLua::Push(bool bValue)
{
	const int nPushIndex = FindEmptyPushIndex();
	if (nPushIndex == -1)
	{
		SoLuaErrorHandle::Print("SoLua::Push : over flow");
		return;
	}
	ms_kPushElementList[nPushIndex].nType = ElementType_bool;
	ms_kPushElementList[nPushIndex].dfValue = bValue ? 1.0 : -1.0;
}
//-----------------------------------------------------------------------------
const char* SoLua::GetString(int nIndex, const char* szDefault)
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
double SoLua::GetDouble(int nIndex, double dfDefault)
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
bool SoLua::GetBool(int nIndex, bool bDefault)
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
//-----------------------------------------------------------------------------
bool SoLua::CallEnd()
{
	if (ms_L == 0)
	{
		return false;
	}

	int nResult = SoLuaResultError;
	lua_pushcfunction(ms_L, &PMode_CallEnd);
	if (lua_pcall(ms_L, 0, 1, 0) == 0)
	{
		nResult = lua_tointeger(ms_L, -1); //取出返回值
		lua_pop(ms_L, 1); //把返回值弹出栈
	}
	else
	{
		HandleLuaAPIError();
	}
	return (nResult == SoLuaResultOK);
}
//-----------------------------------------------------------------------------
bool SoLua::FieldEnd()
{
	if (ms_L == 0)
	{
		return false;
	}

	int nResult = SoLuaResultError;
	lua_pushcfunction(ms_L, &PMode_FieldEnd);
	if (lua_pcall(ms_L, 0, 1, 0) == 0)
	{
		nResult = lua_tointeger(ms_L, -1); //取出返回值
		lua_pop(ms_L, 1); //把返回值弹出栈
	}
	else
	{
		HandleLuaAPIError();
	}
	return (nResult == SoLuaResultOK);
}
//--------------------------------------------------------------------
void SoLua::HandleLuaAPIError()
{
	const char* pszErrorMsg = lua_tostring(ms_L, -1);
	SoLuaErrorHandle::Print("SoLua::HandlePCallError : %s", pszErrorMsg);
	lua_pop(ms_L, 1); //把错误提示信息弹出栈
}
//--------------------------------------------------------------------
int SoLua::PMode_OpenLibs(lua_State* L)
{
	luaL_openlibs(L);
	return 0;
}
//--------------------------------------------------------------------
int SoLua::PMode_ExecuteTrunk(lua_State* L)
{
	int nFuncResult = SoLuaResultError;
	const char* pBuff = (const char*)lua_touserdata(L, 1);
	int nBuffSize = (int)lua_tointeger(L, 2);
	//
	int nResult = luaL_loadbuffer(L, pBuff, nBuffSize, 0); //把字符串内容编译成一个函数，并压入栈
	if (nResult == 0)
	{
		//加载成功
		nResult = lua_pcall(L, 0, 0, 0); //执行lua函数，并把lua函数弹出栈；没有参数，也没有返回值
		if (nResult == 0)
		{
			nFuncResult = SoLuaResultOK;
		}
		else
		{
			HandleLuaAPIError();
		}
	}
	else
	{
		HandleLuaAPIError();
	}
	//压入返回值
	lua_pushinteger(L, nFuncResult);
	return 1;
}
//--------------------------------------------------------------------
int SoLua::PMode_CallEnd(lua_State* L)
{
	//记录已经压入了多少个table
	const int nTableCount = PushTableList();
	if (nTableCount == -1)
	{
		//压入返回值
		lua_pushinteger(L, SoLuaResultError);
		return 1;
	}
	//找到函数名字
	const char* pszFunc = 0;
	const int nFuncNameIndex = nTableCount;
	if (nFuncNameIndex >= 0 && nFuncNameIndex < SoLua_MaxCount_PushElement)
	{
		if (ms_kPushElementList[nFuncNameIndex].nType == ElementType_string_FuncName)
		{
			pszFunc = ms_kPushElementList[nFuncNameIndex].szValue;
		}
	}
	if (pszFunc == 0)
	{
		SoLuaErrorHandle::Print("SoLua::PMode_CallEnd : Can not find the function name");
		lua_pushinteger(L, SoLuaResultError);
		return 1;
	}
	//
	if (nTableCount == 0)
	{
		lua_getglobal(L, pszFunc); //从全局环境中找到pszFunc并压入栈；如果找不到则压入nil
	}
	else
	{
		lua_getfield(L, -1, pszFunc); //从table中找到pszFunc并压入栈；如果找不到则压入nil
	}
	if (lua_isfunction(L, -1) == false)
	{
		lua_pop(L, nTableCount+1); //找不到这个function。把以前的table和刚压入的nil弹出栈
		SoLuaErrorHandle::Print("SoLua::PMode_CallEnd : Can not find the function[%s]", pszFunc);
		lua_pushinteger(L, SoLuaResultError);
		return 1;
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//压入参数
	int nArgCount = 0;
	const int nParamStartIndex = nFuncNameIndex + 1;
	for (int i = nParamStartIndex; i < SoLua_MaxCount_PushElement; ++i)
	{
		const int theType = ms_kPushElementList[i].nType;
		if (theType == ElementType_Invalid)
		{
			break;
		}
		//
		switch (theType)
		{
		case ElementType_double:
			{
				lua_pushnumber(L, ms_kPushElementList[i].dfValue);
				break;
			}
		case ElementType_string:
			{
				lua_pushstring(L, ms_kPushElementList[i].szValue);
				break;
			}
		case ElementType_bool:
			{
				int b = ms_kPushElementList[i].dfValue > 0.0 ? 1 : 0;
				lua_pushboolean(L, b);
				break;
			}
		default:
			{
				SoLuaErrorHandle::Print("SoLua::PMode_CallEnd : invalid element type[%d]", theType);
				break;
			}
		}
		++nArgCount;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//没有办法预知有多少个返回值，所以返回值的个数总是SoLua_MaxCount_PopElement个。
	const int nResultCount = SoLua_MaxCount_PopElement;
	//执行lua函数，并把lua函数弹出栈；
	//如果执行成功则把(nResultCount)个返回值压入栈；
	//如果lua函数的实际返回值少于nResultCount个，则压入nil补全；
	//如果lua函数的实际返回值多于nResultCount个，则截断；
	//如果执行失败则把错误信息压入栈。
	if (lua_pcall(L, nArgCount, nResultCount, 0) != 0)
	{
		//执行失败.
		const char* pszErrorMsg = lua_tostring(L, -1);
		SoLuaErrorHandle::Print("SoLua::PMode_CallEnd : lua_pcall fail [%s][%s]", pszFunc, pszErrorMsg);
		lua_pop(L, nTableCount+1); //把table和刚压入的错误提示信息弹出栈
		lua_pushinteger(L, SoLuaResultError);
		return 1;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//取出返回值
	int nPopCount = 0;
	int nStringValueCount = 0; //记录返回值中字符串类型的个数
	for (int i = -nResultCount; i < 0; ++i)
	{
		//取出value
		CopyResultValue(i, nPopCount, nStringValueCount);
		//
		++nPopCount;
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	lua_pop(L, nTableCount+nResultCount); //把table和返回值弹出栈
	lua_pushinteger(L, SoLuaResultOK);
	return 1;
}
//--------------------------------------------------------------------
int SoLua::PMode_FieldEnd(lua_State* L)
{
	//记录已经压入了多少个table
	const int nTableCount = PushTableList();
	if (nTableCount == -1)
	{
		//压入返回值
		lua_pushinteger(L, SoLuaResultError);
		return 1;
	}
	//记录返回值中字符串类型的个数
	int nStringValueCount = 0;
	const int nParamStartIndex = nTableCount;
	for (int i = nParamStartIndex; i < SoLua_MaxCount_PushElement; ++i)
	{
		if (i >= SoLua_MaxCount_PopElement)
		{
			//返回值的个数已经达到最大个数
			SoLuaErrorHandle::Print("SoLua::PMode_FieldEnd : pop element over flow");
			break;
		}
		const int theType = ms_kPushElementList[i].nType;
		if (theType == ElementType_Invalid)
		{
			//遍历key完毕
			break;
		}
		else if (theType != ElementType_string)
		{
			//key必须是string类型
			SoLuaErrorHandle::Print("SoLua::PMode_FieldEnd : invalid element type[%d]", theType);
			break;
		}
		//
		if (nTableCount == 0)
		{
			lua_getglobal(L, ms_kPushElementList[i].szValue); //先压入key，找到value，然后弹出key并压入value；如果找不到则压入nil
		}
		else
		{
			lua_getfield(L, -1, ms_kPushElementList[i].szValue); //先压入key，找到value，然后弹出key并压入value；如果找不到则压入nil
		}
		//取出value
		CopyResultValue(-1, i, nStringValueCount);
		//
		lua_pop(L, 1); //把value弹出栈
	}
	//
	lua_pop(L, nTableCount); //把所有的table弹出栈
	lua_pushinteger(L, SoLuaResultOK);
	return 1;
}
//--------------------------------------------------------------------
bool SoLua::LoadFileToBuff(const char* pszFile, char** ppBuff, int& nBuffSize)
{
	if (pszFile==0 || pszFile[0]==0 || ppBuff==0)
	{
		return false;
	}
	//打开文件.
	FILE* fp = fopen(pszFile, "rb");
	if (fp == 0)
	{
		return false;
	}
	//计算文件大小.
	fseek(fp, 0, SEEK_END);
	nBuffSize = (int)ftell(fp);
	//分配Buff内存.
	*ppBuff = (char*)malloc(nBuffSize);
	if (*ppBuff == 0)
	{
		//申请内存失败。
		fclose(fp);
		nBuffSize = 0;
		return false;
	}
	//填充Buff。
	fseek(fp, 0, SEEK_SET);
	const size_t sizeFileSize = (size_t)nBuffSize;
	size_t nActuallyRead = fread(*ppBuff, 1, sizeFileSize, fp);
	if (nActuallyRead != sizeFileSize)
	{
		free(*ppBuff);
		*ppBuff = 0;
		nBuffSize = 0;
		fclose(fp);
		return false;
	}
	//关闭文件。
	fclose(fp);
	return true;
}
//-----------------------------------------------------------------------------
void SoLua::PushClear()
{
	for (int i = 0; i < SoLua_MaxCount_PushElement; ++i)
	{
		ms_kPushElementList[i].nType = ElementType_Invalid;
	}
}
//-----------------------------------------------------------------------------
int SoLua::PushTableList()
{
	//记录已经压入了多少个table
	int nTableCount = 0;
	for (int i = 0; i < SoLua_MaxCount_PushElement; ++i)
	{
		if (ms_kPushElementList[i].nType != ElementType_string_TableName)
		{
			break;
		}
		const char* szTableName = ms_kPushElementList[i].szValue;
		if (nTableCount == 0)
		{
			lua_getglobal(ms_L, szTableName); //从全局环境中找到szTableName并压入栈；如果找不到则压入nil
		}
		else
		{
			lua_getfield(ms_L, -1, szTableName); //从前一个table中找到szTableName并压入栈；如果找不到则压入nil
		}
		//
		if (lua_istable(ms_L, -1) == true)
		{
			++nTableCount;
		}
		else
		{
			lua_pop(ms_L, nTableCount+1); //找不到szTableName，把以前的table和刚压入的nil弹出栈
			SoLuaErrorHandle::Print("SoLua::PushTableList : Can not find the table[%s]", szTableName);
			nTableCount = -1;
			break;
		}
	}
	return nTableCount;
}
//-----------------------------------------------------------------------------
bool SoLua::CopyResultValue(const int nStackIndex, const int nPopElementIndex, int& nStringValueCount)
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
			//深拷贝
			if (nStringValueCount < SoLua_MaxStringCount)
			{
				strncpy(ms_szStringValueList[nStringValueCount], szValue, SoLua_MaxStringSize);
				ms_szStringValueList[nStringValueCount][SoLua_MaxStringSize-1] = 0;
				ms_kPopElementList[nPopElementIndex].szValue = ms_szStringValueList[nStringValueCount];
				++nStringValueCount;
			}
			else
			{
				ms_kPopElementList[nPopElementIndex].szValue = "";
				br = false;
				SoLuaErrorHandle::Print("SoLua::CopyResultValue : output string value count too much");
			}
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
			SoLuaErrorHandle::Print("SoLua::CopyResultValue : invalid output value type[%s]", szTypeName);
			break;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
int SoLua::FindEmptyPushIndex()
{
	int nPushIndex = -1;
	for (int i = 0; i < SoLua_MaxCount_PushElement; ++i)
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
