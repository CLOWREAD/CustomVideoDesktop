//CustomDesktop.cpp: 定义应用程序的入口点。
//

#include "header.h"
#include "CustomDesktop.h"
#include "MpegDecode.h"
#include "BF_PaintDC.h"
#define MAX_LOADSTRING 100
#include "FindDeskCanvas.h"
#include "ReadConfig.h"
#include <Windows.h>
#include <shellapi.h>
FindDeskCanvas m_FDC;
MpegDecode m_MD;
// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
BF_PaintDC m_BPD;
HWND m_HWND;
HINSTANCE m_HINSTANCE;
ReadConfig m_RC;
// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Init();
void UpdateFrame();
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CUSTOMDESKTOP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CUSTOMDESKTOP));

	Init();


    MSG msg;
	SetTimer(m_HWND, 1, 60, 0);
    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

void UpdateFrame()
{

	char str[32];
	 MpegDecode::IMGFrame *imgf=&(m_MD.m_CurrentFrame);
	 if (m_BPD.m_H == imgf->height && m_BPD.m_W == imgf->width)
	 {

	 }
	 else
	 {
		 m_BPD.m_H = imgf->height;
		 m_BPD.m_W = imgf->width;
		 m_BPD.m_LineSize = imgf->linesize;

		 delete m_BPD.m_Data;
		 m_BPD.m_Data = new char[m_BPD.m_LineSize*m_BPD.m_H];
		 m_BPD.Init(m_FDC.m_Canvas);
	 }
	
	 memcpy(m_BPD.m_Data, imgf->data, m_BPD.m_LineSize*m_BPD.m_H);

	//m_BPD.Init(m_FDC.m_Canvas);
	

}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CUSTOMDESKTOP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CUSTOMDESKTOP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 256, 128, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, false);
   UpdateWindow(hWnd);
   m_HWND = hWnd;
   m_HINSTANCE = hInstance;
   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int res;
    switch (message)
    {
	case WM_CREATE:
		m_BPD.Init(m_FDC.m_Canvas);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_TIMER:
		res=m_MD.NextFrame();
		if (res)
		{
			
			UpdateFrame();
			HWND h = m_FDC.GetCanvas();
			m_BPD.Paint(h);
		}
		else
		{
			m_MD.End();
			m_MD.Reset(m_RC.m_VideoFilePath);
		}

		
		break;
    case WM_PAINT:
        {
		    
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
			
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_USER + 1024:
		switch (lParam)

		{

		case WM_RBUTTONUP:
			exit(0);
			break;

		}
		
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Init()
{
	m_RC.Read();
	m_FDC.GetCanvas();
	m_MD.Reset(m_RC.m_VideoFilePath);
	UpdateFrame();

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_HWND;
	nid.uID = IDI_SMALL;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER+1024;//自定义的消息名称
	nid.hIcon = LoadIconW(m_HINSTANCE, MAKEINTRESOURCE(IDI_SMALL));

	lstrcpyW(nid.szTip, _T("计划任务提醒"));
	Shell_NotifyIcon(NIM_ADD, &nid);

}