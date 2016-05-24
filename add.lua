----add.lua--------


--printddd(MaxWindowID)


globalString = "globalStringkk1"
kkjjf = 
{
	localStringKey = "localStringpp2",
}

print(MaxWindowID)
print("WindowType_Button_" .. eWindowType.WindowType_Button)
print("WindowType_Text_" .. eWindowType.WindowType_Text)

function add( x, y )
	local width = luaGetWindowWidth(111)
	print(width)
	luaSetWindowWidth(111, 33)
	local width2, height2 = luaGetWindowWidthHeight(111)
	print(width2)
	print(height2)
	local windowText = luaGetWindowText(1)
	print(windowText)
	luaSetWindowText(111, "kkk")
	
	local _posX = WindowHelp.GetWindowPosX(111)
	print("WindowHelp_" .. _posX)
	
	local _texture = WindowHelp.GetWindowTexture(33)
	print("WindowHelp_" .. _texture)
end

adddd =
{
	add = function(x,y,z) return x*y, z end,

	ccc = 
	{
		add2 = function(x,y,z) return x*y, z, luaGetWindowIDByName("oil") end,
		kk = "jjd",
		kk33 = 33,
	}
}
