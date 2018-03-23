#pragma once
#include "windows.h"
class BF_PaintDC
{
public:
	BF_PaintDC();
	~BF_PaintDC();
	HDC m_MainDC;
	HDC m_PaintDC;
	HBITMAP m_HBmp;
	HANDLE m_OldHandle;
	HWND m_HWnd;
	RECT m_Rect;
	SIZE m_WindowSize;
	void Init(HWND hwnd);
	int m_W,m_LineSize, m_H,m_Size;
	char * m_Data;
	int m_ImgIndex = 100;
	void Paint(HWND hwnd);
};

