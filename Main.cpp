//--------------------------------------------------------------------
#include "SoLua.h"
#include "SoLuaBind.h"
#include "LuaBind_MyFunc.h"
//--------------------------------------------------------------------
void main()
{
	SoLua::InitLua();
	SoLuaBind::InitLuaBind(SoLua::GetLua());
	//
	SoLuaBind::ExecuteBind(luaRegisterMyFunc);
	SoLua::DumpStack();
	//
	SoLua::ExecuteFile("add.lua");
	SoLua::DumpStack();

	int nExeResult = 0;
	SoLua::CallBegin();
	SoLua::PushFunc("add");
	SoLua::CallEnd();
	SoLua::DumpStack();
	//
	SoLua::FieldBegin();
	SoLua::Push("globalString");
	SoLua::FieldEnd();
	const char* szglobalString = SoLua::GetString(0, "");
	//
	SoLua::FieldBegin();
	SoLua::PushTable("kkjjf");
	SoLua::Push("localStringKey");
	SoLua::FieldEnd();
	const char* szlocalString = SoLua::GetString(0, "");

	//
	SoLua::ExecuteChunk("print(\"oil\")", 0);
	SoLua::DumpStack();

	short ddd = 2;
	SoLua::CallBegin();
	SoLua::PushTable("adddd");
	SoLua::PushTable("ccc");
	SoLua::PushFunc("add2");
	SoLua::Push(2.5f);
	SoLua::Push(4.7);
	SoLua::Push("oil");
	SoLua::Push(ddd);
	SoLua::CallEnd();
	double dfReturn = SoLua::GetDouble(0, 0);
	const char* szReturn = SoLua::GetString(1, "");

	SoLua::FieldBegin();
	SoLua::PushTable("adddd");
	SoLua::PushTable("ccc");
	SoLua::Push("kk");
	SoLua::Push("kk33");
	SoLua::FieldEnd();
	SoLua::DumpStack();
	szReturn = SoLua::GetString(0, "");
	dfReturn = SoLua::GetDouble(1, 0);

	SoLua::DumpStack();
	SoLuaBind::ReleaseLuaBind();
	SoLua::ReleaseLua();
}
