#include <iostream>
#include <easyx.h>
#include <bitset>

struct PT_Piece 
{
	int Width;	// ��
	int Height;	// ��
	int Z_Height; // �߶�
	COORD Pos;	// λ��
	double RotateDegree; // ��ת�Ƕ�
	IMAGE* Stored; // ����ͼƬ���� new �� delete ����
	IMAGE* Shown;	// ������ʾ��ͼƬ
};

#define _WINDOW_WIDTH_  1280
#define _WINDOW_HEIGHT_ 720

int main(int argc, wchar_t** argv, wchar_t** envp)
{
	HWND GraphicHandle = initgraph(_WINDOW_WIDTH_, _WINDOW_HEIGHT_, EW_SHOWCONSOLE);
	
	// �ı䴰����ʽ��λ�ã����У�
	LONG lStyle = WS_VISIBLE | WS_POPUP | WS_BORDER;
	SetWindowLong(GraphicHandle, GWL_STYLE, lStyle);

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(GraphicHandle, NULL, (ScreenWidth - _WINDOW_WIDTH_) / 2, (ScreenHeight - _WINDOW_HEIGHT_) / 2, _WINDOW_WIDTH_, _WINDOW_HEIGHT_, SWP_SHOWWINDOW);

	// ��������
	setbkcolor(0xFFFFFF);
	setfillcolor(0xFFFFFF);

	Sleep(5000);
	

	return 0;
}