#include "FindDeskCanvas.h"

#include <wchar.h>
#include <tchar.h>

FindDeskCanvas::FindDeskCanvas()
{
}


FindDeskCanvas::~FindDeskCanvas()
{
}

HWND FindDeskCanvas::GetCanvas()
{
	
	HWND h = FindWindow(L"WorkerW",0);
	
	while (HasChild(h))
	{
		h = FindWindowExW(0, h, L"WorkerW", 0);
	}

	

	m_Canvas = h;
	return h;

}

bool FindDeskCanvas::HasChild(HWND h)
{
	if (h == (HWND)0x10240)
	{
		OutputDebugString(_T(" "));
	}
	TCHAR str[32];
	wsprintf(str, _T("%x \n"), h);
	OutputDebugString(str);
	int l = GetWindowLongW(h, GWL_STYLE);
	bool iv = l & WS_VISIBLE;
	if (iv== 0)
	{
		return true;
	}

	HWND hr= FindWindowExW(h, 0, L"SHELLDLL_DefView", 0);
	if (hr>0)
	{
		return true;
	}

	return false;
}
