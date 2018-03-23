#pragma once
#include <windows.h>
class FindDeskCanvas
{
public:
	HWND m_Canvas;
	FindDeskCanvas();
	~FindDeskCanvas();
	HWND GetCanvas();
	bool HasChild(HWND h);

};

