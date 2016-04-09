//-----------------------------------------------------------------------------
#ifndef _SoLuaErrorHandle_h_
#define _SoLuaErrorHandle_h_
//-----------------------------------------------------------------------------
#define SoLuaErrorHandle_BuffSize 1024
//-----------------------------------------------------------------------------
class SoLuaErrorHandle
{
public:
	static void Print(const char* szFormat, ...);

private:
	static char ms_szPrintBuff[SoLuaErrorHandle_BuffSize];
};
//-----------------------------------------------------------------------------
#endif //_SoLuaErrorHandle_h_
//-----------------------------------------------------------------------------
