//-----------------------------------------------------------------------------
#include "MyFunc.h"
//-----------------------------------------------------------------------------
int GetWindowIDByName(const char* szWindowName)
{
	return 11;
}
//-----------------------------------------------------------------------------
float GetWindowWidth(int nWindowID)
{
	if (nWindowID == 111)
	{
		return 111.0f;
	}
	else
	{
		return 77.0f;
	}
}
//-----------------------------------------------------------------------------
void SetWindowWidth(int nWindowID, float fWidth)
{
	if (nWindowID == 111)
	{
		fWidth += 111.0f;
	}
	else
	{
		fWidth += 77.0f;
	}
}
//-----------------------------------------------------------------------------
void GetWindowWidthHeight(int nWindowID, float& fWidth, float& fHeight)
{
	if (nWindowID == 111)
	{
		fWidth = 111.0f;
		fHeight = 222.0f;
	}
	else
	{
		fWidth = 77.0f;
		fHeight = 88.0f;
	}
}
//-----------------------------------------------------------------------------
void SetWindowText(int nWindowID, const char* szText)
{
	if (nWindowID == 111)
	{
		szText = "";
	}
	else
	{
		szText = "";
	}
}
//-----------------------------------------------------------------------------
const char* GetWindowText(int nWindowID)
{
	if (nWindowID == 111)
	{
		return "text111";
	}
	else
	{
		return "text43";
	}
}
//-----------------------------------------------------------------------------
float WindowHelp::GetWindowPosX(int nWindowID)
{
	if (nWindowID == 111)
	{
		return 111.0f;
	}
	else
	{
		return 33.0f;
	}
}
//-----------------------------------------------------------------------------
void WindowHelp::SetWindowPosX(int nWindowID, float fPosX)
{
	if (nWindowID == 111)
	{
		fPosX = 222.0f;
	}
	else
	{
		fPosX = 33.0f;
	}
}
//-----------------------------------------------------------------------------
void WindowHelp::GetWindowPos(int nWindowID, float& fPosX, float& fPosY)
{
	if (nWindowID == 111)
	{
		fPosX = 111.0f;
		fPosY = 222.0f;
	}
	else
	{
		fPosX = 77.0f;
		fPosY = 88.0f;
	}
}
//-----------------------------------------------------------------------------
void WindowHelp::SetWindowTexture(int nWindowID, const char* szTexture)
{
	if (nWindowID == 111)
	{
		szTexture = "";
	}
	else
	{
		szTexture = "";
	}
}
//-----------------------------------------------------------------------------
const char* WindowHelp::GetWindowTexture(int nWindowID)
{
	if (nWindowID == 111)
	{
		return "Texture111";
	}
	else
	{
		return "Texture43";
	}
}
//-----------------------------------------------------------------------------
