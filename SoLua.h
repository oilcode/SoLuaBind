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
//调用CallBegin()或者FieldBegin()时，最多压入 SoLua_MaxCount 个参数，这些参数包括table名字，函数名字和函数参数，此时lua栈被占用的长度是(SoLua_MaxCount)；
//调用CallBegin()或者FieldBegin()时，最多有 SoLua_MaxCount 个返回值，此时lua栈被占用的长度是 (参数中table的个数 + SoLua_MaxCount)；
//lua栈的默认长度是20，要保证(参数中table的个数 + SoLua_MaxCount)不大于lua栈长度。
#define SoLua_MaxCount 14
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
	static bool ExecuteChunk(const char* pBuff, int nBuffSize);
	//把当前的lua栈打印出来。
	//注意，这个函数不能用保护模式，否则，执行lua_pcall后就进入了一个新的lua栈，这不是我们想打印的lua栈。
	static void DumpStack();
	static void SetHandleError(bool bEnable);

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	static void PushTable(const char* szTableName);
	static void PushFunc(const char* szFuncName);
	static void Push(const char* szValue);
	static void Push(double dfValue);
	static void Push(bool bValue);
	static void Push(float fValue);
	static void Push(unsigned int uiValue);
	static void Push(int nValue);
	//取出返回值。序号从0开始。
	static const char* GetString(int nIndex, const char* szDefault="");
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
		const char* szValue; //字符串都是浅拷贝
		eElementType nType;
		//
		stElement():nType(ElementType_Invalid) {};
	};

private:
	//lua API函数执行失败的错误处理。
	static void HandleLuaAPIError();
	//以保护模式执行本函数，返回值表示压入栈内的元素的个数。
	static int PMode_OpenLibs(lua_State* L);
	static int PMode_ExecuteChunk(lua_State* L);
	static int PMode_CallEnd(lua_State* L);
	static int PMode_FieldEnd(lua_State* L);
	//从磁盘上打开指定的文件，把文件内容读取到指定的Buff内。
	//函数内部会为Buff分配内存，由外界负责删除Buff。
	static bool LoadFileToBuff(const char* pszFile, char** ppBuff, int* pBuffSize);
	//依次压入table名字，返回压入了多少个table。
	//返回-1表示失败了。
	static int PushTableList();
	//把栈中的第nStackIndex个元素的值拷贝到pElement中。
	static bool CopyResultValue(const int nStackIndex, stElement* pElement);

private:
	//lua环境。
	static lua_State* ms_L;
	//在压入参数阶段，此列表内存储参数；在取出返回值阶段，此列表内存储返回值。
	static stElement ms_kElementList[SoLua_MaxCount];
	//记录 ms_kElementList 中有效元素的个数。
	static int ms_nSize;
	//当遇到lua API报错时，是否打印错误信息。
	static bool ms_bHandleError;
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
	ms_nSize = 0;
}
//--------------------------------------------------------------------
inline void SoLua::FieldBegin()
{
	ms_nSize = 0;
}
//-----------------------------------------------------------------------------
#endif //_SoLua_h_
//-----------------------------------------------------------------------------
