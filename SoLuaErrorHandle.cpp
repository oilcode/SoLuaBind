//-----------------------------------------------------------------------------
#include "SoLuaErrorHandle.h"
#include <stdio.h>
#include <Windows.h>
//-----------------------------------------------------------------------------
char SoLuaErrorHandle::ms_szPrintBuff[SoLuaErrorHandle_BuffSize];
//-----------------------------------------------------------------------------
void SoLuaErrorHandle::Print(const char* szFormat, ...)
{
	va_list klist;
	va_start(klist, szFormat);
	vsnprintf(ms_szPrintBuff, SoLuaErrorHandle_BuffSize, szFormat, klist);
	va_end(klist);
	//
	OutputDebugStringA(ms_szPrintBuff);
	OutputDebugStringA("\n");
}
//-----------------------------------------------------------------------------
