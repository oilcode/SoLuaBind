//-----------------------------------------------------------------------------
#ifndef _MyFunc_h_
#define _MyFunc_h_
//-----------------------------------------------------------------------------
#define MaxWindowID 9999
//-----------------------------------------------------------------------------
enum eWindowType
{
	WindowType_Button,
	WindowType_Text,
};
//-----------------------------------------------------------------------------
int GetWindowIDByName(const char* szWindowName);
float GetWindowWidth(int nWindowID);
void SetWindowWidth(int nWindowID, float fWidth);
void GetWindowWidthHeight(int nWindowID, float& fWidth, float& fHeight);
void SetWindowText(int nWindowID, const char* szText);
const char* GetWindowText(int nWindowID);

class WindowHelp
{
public:
	static float GetWindowPosX(int nWindowID);
	static void SetWindowPosX(int nWindowID, float fPosX);
	static void GetWindowPos(int nWindowID, float& fPosX, float& fPosY);
	static void SetWindowTexture(int nWindowID, const char* szTexture);
	static const char* GetWindowTexture(int nWindowID);
};
//-----------------------------------------------------------------------------
#endif //_MyFunc_h_
//-----------------------------------------------------------------------------
