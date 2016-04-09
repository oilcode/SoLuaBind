//-----------------------------------------------------------------------------
#ifndef _SoLuaBind_h_
#define _SoLuaBind_h_
//-----------------------------------------------------------------------------
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
//-----------------------------------------------------------------------------
//每次执行lua栈操作时，最多压入多少个参数
#define SoLuaBind_MaxCount_PushElement 10
//每次执行lua栈操作时，最多有多少个返回值
#define SoLuaBind_MaxCount_PopElement 10
//-----------------------------------------------------------------------------
class SoLuaBind
{
public:
	//初始化本模块。
	static void InitLuaBind(lua_State* L);
	//释放本模块。
	static void ReleaseLuaBind();

	//执行用户写的绑定逻辑。
	static void ExecuteBind(lua_CFunction pBindFunc);

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//在 lua_CFunction 中使用这里的一系列的函数。
	//从lua栈中获取所有的函数参数，存储到 ms_kPopElementList 中
	static bool FuncBegin();
	//获取 ms_kPopElementList 中的元素。序号从0开始。
	//这些函数主要用于获取函数参数。
	static double GetDouble(int nIndex, double dfDefault=0.0f);
	static const char* GetString(int nIndex, const char* szDefault="");
	static bool GetBool(int nIndex, bool bDefault=false);
	//依次逐个压入函数返回值
	static void Return(double dfValue);
	static void Return(float fValue);
	static void Return(unsigned int uiValue);
	static void Return(int nValue);
	static void Return(const char* szValue);
	static void Return(bool bValue);
	//获取函数返回值的个数，绑定结束
	static int FuncEnd();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//向lua环境中注册C++函数
	static void RegFunction(const char* szFunctionName, lua_CFunction pFunction);
	static void RegLib(const char* szLibName, const luaL_Reg* pLibReg);
	
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//下面这些函数用于注册数组和哈希表。
	//把key变量压入到 ms_kPushElementList 中。
	static void PushKey(double dfValue);
	static void PushKey(float fValue);
	static void PushKey(unsigned int uiValue);
	static void PushKey(int nValue);
	static void PushKey(const char* szValue); //注意，字符串szValue是浅拷贝。
	static void PushKey(bool bValue);
	//把value变量压入到 ms_kPopElementList 中。
	static void PushValue(double dfValue);
	static void PushValue(float fValue);
	static void PushValue(unsigned int uiValue);
	static void PushValue(int nValue);
	static void PushValue(const char* szValue); //注意，字符串szValue是浅拷贝。
	static void PushValue(bool bValue);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//注册一个全局的数组型的table。
	//在lua中，数组的下标从1开始，本函数遵循这个规定。
	static void ArrayBegin();
	//--szArrayName 在lua环境中，该table的名字。
	//--nElementCount 数组中元素的最大个数。用于预分配内存空间。如果不知道最大个数，可以传入0。
	static bool ArrayEnd(const char* szArrayName, int nElementCount);

	//注册一个全局的哈希型的table。
	//key可以是数字，也可以是字符串。
	static void HashBegin();
	//--szHashName 在lua环境中，该table的名字。
	//--nElementCount table中键值对的最大个数。用于预分配内存空间。如果不知道最大个数，可以传入0。
	static bool HashEnd(const char* szHashName, int nElementCount);

private:
	static bool IsValidPopIndex(int nIndex);
	//把栈中的第nStackIndex个元素拷贝到本类中的第nPopElementIndex个ms_kPopElementList元素。	
	static bool CopyResultValue(const int nStackIndex, const int nPopElementIndex);
	//压入变量到 ms_kPushElementList 时，获取存储该变量的stElement的序号。
	//返回-1表示没有找到空闲的stElement。
	static int FindEmptyPushIndex();
	//压入变量到 ms_kPopElementList 时，获取存储该变量的stElement的序号。
	//返回-1表示没有找到空闲的stElement。
	static int FindEmptyPopIndex();
	//清除临时数据。
	static void ClearTemp();

private:
	enum eElementType
	{
		ElementType_Invalid,
		ElementType_double,
		ElementType_string,
		ElementType_bool,
	};
	struct stElement
	{
		double dfValue;
		const char* szValue;
		eElementType nType;
		//
		stElement():nType(ElementType_Invalid) {};
	};

private:
	//lua环境
	static lua_State* ms_L;
	//依次存储参数
	static stElement ms_kPushElementList[SoLuaBind_MaxCount_PushElement];
	//依次存储返回值
	static stElement ms_kPopElementList[SoLuaBind_MaxCount_PopElement];
	//在lua绑定逻辑中使用的临时变量
	static int ms_nBindResultCount;

};
//-----------------------------------------------------------------------------
inline void SoLuaBind::Return(double dfValue)
{
	lua_pushnumber(ms_L, dfValue);
	++ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::Return(float fValue)
{
	lua_pushnumber(ms_L, (double)fValue);
	++ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::Return(unsigned int uiValue)
{
	lua_pushnumber(ms_L, (double)uiValue);
	++ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::Return(int nValue)
{
	lua_pushnumber(ms_L, (double)nValue);
	++ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::Return(const char* szValue)
{
	lua_pushstring(ms_L, szValue);
	++ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::Return(bool bValue)
{
	lua_pushboolean(ms_L, bValue ? 1 : 0);
	++ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline int SoLuaBind::FuncEnd()
{
	return ms_nBindResultCount;
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::RegFunction(const char* szFunctionName, lua_CFunction pFunction)
{
	lua_register(ms_L, szFunctionName, pFunction);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::RegLib(const char* szLibName, const luaL_Reg* pLibReg)
{
	luaL_register(ms_L, szLibName, pLibReg); //新table被留在了栈中
	lua_pop(ms_L, 1); //把新table弹出栈
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::PushKey(float fValue)
{
	PushKey((double)fValue);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::PushKey(unsigned int uiValue)
{
	PushKey((double)uiValue);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::PushKey(int nValue)
{
	PushKey((double)nValue);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::PushValue(float fValue)
{
	PushValue((double)fValue);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::PushValue(unsigned int uiValue)
{
	PushValue((double)uiValue);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::PushValue(int nValue)
{
	PushValue((double)nValue);
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::ArrayBegin()
{
	ClearTemp();
}
//-----------------------------------------------------------------------------
inline void SoLuaBind::HashBegin()
{
	ClearTemp();
}
//-----------------------------------------------------------------------------
inline bool SoLuaBind::IsValidPopIndex(int nIndex)
{
	return (nIndex >= 0 && nIndex < SoLuaBind_MaxCount_PopElement);
}
//-----------------------------------------------------------------------------
#endif //_SoLuaBind_h_
//-----------------------------------------------------------------------------
