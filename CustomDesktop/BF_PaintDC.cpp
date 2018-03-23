#include "BF_PaintDC.h"
#include "windows.h"
#include <stdlib.h>
#include <uchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BF_PaintDC::BF_PaintDC()
{
}


BF_PaintDC::~BF_PaintDC()
{
}

void BF_PaintDC::Init(HWND hwnd)
{
	m_HWnd = hwnd;

	
	GetWindowRect(m_HWnd, &m_Rect);

	m_MainDC = GetDC(m_HWnd);

	m_WindowSize.cx= GetDeviceCaps(m_MainDC, DESKTOPHORZRES);
	m_WindowSize.cy= GetDeviceCaps(m_MainDC, DESKTOPVERTRES);
	
	m_PaintDC = CreateCompatibleDC(m_MainDC);
	m_HBmp = CreateCompatibleBitmap(m_MainDC, m_W, m_H);
	m_OldHandle = SelectObject(m_PaintDC, m_HBmp);
	//SelectObject(m_PaintDC, oldhandle);
}

void BF_PaintDC::Paint(HWND hwnd)
{
	

	SelectObject(m_PaintDC, m_OldHandle);
	long s = m_H * m_LineSize;
	SetBitmapBits(m_HBmp, s,m_Data);
	m_OldHandle = SelectObject(m_PaintDC, m_HBmp);
	StretchBlt(m_MainDC, 0, 0, m_WindowSize.cx, m_WindowSize.cy, m_PaintDC, 0, 0,m_W,m_H ,SRCCOPY);
	

}
