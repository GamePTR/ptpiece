#include <iostream>
#include <easyx.h>
#include <bitset>

struct PT_Piece 
{
	int Width;	// 长
	int Height;	// 高
	int Z_Height; // 高度
	COORD Pos;	// 位置
	double RotateDegree; // 旋转角度
	IMAGE* Stored; // 所属图片，以 new 和 delete 操作
	IMAGE* Shown;	// 对外显示的图片
};

#define _WINDOW_WIDTH_  1280
#define _WINDOW_HEIGHT_ 720

int main(int argc, wchar_t** argv, wchar_t** envp)
{
	HWND GraphicHandle = initgraph(_WINDOW_WIDTH_, _WINDOW_HEIGHT_, EW_SHOWCONSOLE);
	
	// 改变窗口样式与位置（居中）
	LONG lStyle = WS_VISIBLE | WS_POPUP | WS_BORDER;
	SetWindowLong(GraphicHandle, GWL_STYLE, lStyle);

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(GraphicHandle, NULL, (ScreenWidth - _WINDOW_WIDTH_) / 2, (ScreenHeight - _WINDOW_HEIGHT_) / 2, _WINDOW_WIDTH_, _WINDOW_HEIGHT_, SWP_SHOWWINDOW);

	// 正经代码
	setbkcolor(0xFFFFFF);
	setfillcolor(0xFFFFFF);

	Sleep(5000);
	

	return 0;
}