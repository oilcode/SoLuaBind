//-----------------------------------------------------------------------------
#ifndef _SoLua_h_
#define _SoLua_h_
//-----------------------------------------------------------------------------
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
//-----------------------------------------------------------------------------
//每次执行lua栈操作时，最多压入多少个参数（这些参数包括table名字，函数名字和函数参数）。
#define SoLua_MaxCount_PushElement 14
//每次执行lua栈操作时，最多有多少个返回值。
#define SoLua_MaxCount_PopElement 10
//lua函数的返回值允许是字符串类型的值。
//该字符串会拷贝到本类维护的buff内，供外界使用。
//本类维护的buff会重复利用，外界要尽快取走buff内的字符串。
//本类维护多少个buff，也即lua函数最多有几个字符串类型的返回值。
#define SoLua_MaxStringCount 10
//每个buff内有多少个字节。
#define SoLua_MaxStringSize 1024
//-----------------------------------------------------------------------------
class SoLua
{
public:
	//初始化lua环境。
	static bool InitLua();
	//释放lua环境。
	static void ReleaseLua();
	//获取lua环境。
	static lua_State* GetLua();

	//载入并执行一个lua文件。
	static bool ExecuteFile(const char* pszLuaFile);
	//执行一个代码块。可以是文本代码块，也可以是二进制代码块。
	//如果nBuffSize值为0，则认为pBuff是一个带结束符的字符串，本函数内部会计算字符串长度。
	static bool ExecuteTrunk(const char* pBuff, int nBuffSize);
	//把当前的lua栈打印出来。
	//注意，这个函数不能用保护模式，否则，执行lua_pcall后就进入了一个新的lua栈，这不是我们想打印的lua栈。
	static void DumpStack();

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	static void PushTable(const char* szTableName); //注意，字符串szTableName是浅拷贝。
	static void PushFunc(const char* szFuncName); //注意，字符串szFuncName是浅拷贝。
	static void Push(const char* szValue); //注意，字符串szValue是浅拷贝。
	static void Push(double dfValue);
	static void Push(bool bValue);
	static void Push(float fValue);
	static void Push(unsigned int uiValue);
	static void Push(int nValue);
	//取出返回值。序号从0开始。
	static const char* GetString(int nIndex, const char* szDefault=""); //返回的字符串已经保存在本类内，深拷贝。
	static double GetDouble(int nIndex, double dfDefault=0.0);
	static bool GetBool(int nIndex, bool bDefault=false);
	static float GetFloat(int nIndex, float fDefault=0.0f);
	static unsigned int GetUInt(int nIndex, unsigned int uiDefault=0);
	static int GetInt(int nIndex, int nDefault=0);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//执行一个（table内的）lua函数。
	static void CallBegin();
	static bool CallEnd();
	//获取若干个（table内的）键值对中的value。
	static void FieldBegin();
	static bool FieldEnd();
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

private:
	//lua API函数执行失败的错误处理。
	static void HandleLuaAPIError();
	//以保护模式执行本函数，返回值表示压入栈内的元素的个数。
	static int PMode_OpenLibs(lua_State* L);
	static int PMode_ExecuteTrunk(lua_State* L);
	static int PMode_CallEnd(lua_State* L);
	static int PMode_FieldEnd(lua_State* L);
	//从磁盘上打开指定的文件，把文件内容读取到指定的Buff内。
	//函数内部会为Buff分配内存，由外界负责删除Buff。
	static bool LoadFileToBuff(const char* pszFile, char** ppBuff, int& nBuffSize);
	//把压入的参数清零。
	static void PushClear();
	//依次压入table名字，返回压入了多少个table。
	//返回-1表示失败了。
	static int PushTableList();
	//把栈中的第nStackIndex个元素拷贝到本类中的第nPopElementIndex个ms_kPopElementList元素。
	//--bDeepCopy 是否为深拷贝。
	static bool CopyResultValue(const int nStackIndex, const int nPopElementIndex, int& nStringValueCount);
	//压入函数参数时，获取存储该参数的stElement的序号。
	//返回-1表示没有找到空闲的stElement。
	static int FindEmptyPushIndex();
	//判断是否为有效的ms_kPopElementList数组下标。
	static bool IsValidPopIndex(int nIndex);

private:
	enum eElementType
	{
		ElementType_Invalid,
		ElementType_string_TableName, //该值表示一个table的名字
		ElementType_string_FuncName, //该值表示一个lua脚本函数的名字
		ElementType_string,
		ElementType_double,
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
	static stElement ms_kPushElementList[SoLua_MaxCount_PushElement];
	//依次存储返回值
	static stElement ms_kPopElementList[SoLua_MaxCount_PopElement];
	//存储返回值中的字符串类型的返回值
	static char ms_szStringValueList[SoLua_MaxStringCount][SoLua_MaxStringSize];
};
//-----------------------------------------------------------------------------
inline lua_State* SoLua::GetLua()
{
	return ms_L;
}
//--------------------------------------------------------------------
inline void SoLua::Push(float fValue)
{
	Push((double)fValue);
}
//--------------------------------------------------------------------
inline void SoLua::Push(unsigned int uiValue)
{
	Push((double)uiValue);
}
//--------------------------------------------------------------------
inline void SoLua::Push(int nValue)
{
	Push((double)nValue);
}
//-----------------------------------------------------------------------------
inline float SoLua::GetFloat(int nIndex, float fDefault)
{
	return (float)(GetDouble(nIndex, (double)fDefault));
}
//-----------------------------------------------------------------------------
inline unsigned int SoLua::GetUInt(int nIndex, unsigned int uiDefault)
{
	return (unsigned int)(GetDouble(nIndex, (double)uiDefault));
}
//-----------------------------------------------------------------------------
inline int SoLua::GetInt(int nIndex, int nDefault)
{
	return (int)(GetDouble(nIndex, (double)nDefault));
}
//--------------------------------------------------------------------
inline void SoLua::CallBegin()
{
	PushClear();
}
//--------------------------------------------------------------------
inline void SoLua::FieldBegin()
{
	PushClear();
}
//-----------------------------------------------------------------------------
inline bool SoLua::IsValidPopIndex(int nIndex)
{
	return (nIndex >= 0 && nIndex < SoLua_MaxCount_PopElement);
}
//-----------------------------------------------------------------------------
#endif //_SoLua_h_
//-----------------------------------------------------------------------------
