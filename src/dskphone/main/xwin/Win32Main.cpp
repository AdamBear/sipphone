// Win32Main.cpp : 定义应用程序的入口点。
//

#ifdef _WIN32
#include "stdafx.h"
#include "Win32Main.h"
#include "mainstation.h"
#include "interfacedefine.h"
#include "winimp/winimp.h"
#include "commonunits/modcommonunits.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND g_hMainWnd = nullptr;                        // 主窗口
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
    SetPhoneType(28);
    comminUnits_DeleteInitInfofile();
    comminUnits_WriteInitInfo("InitTime_LibLoadSucceed = 1");

    CMainStation& objStation = CMainStation::getInstance();

    objStation.Init(lpCmdLine, nCmdShow);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32MAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    objStation.m_strThreadName = VP_PHONE_THREAD_NAME;
    objStation.AttachCurrentThread();

    int nResult = objStation.Run();

    objStation.DetachThread();

    return nResult;
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32MAIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
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

#if 0
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
#else
    //POPUP无边框
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP/*WS_CAPTION*/,
                              CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
                              hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hMainWnd = hWnd;

    // 屏幕中央显示
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    //int iWndWidth = LCD_WIDTH + 8;
    //int iWndHeight = LCD_HEIGHT + 8;
    int nWndWidth = 480;
    int nWndHeight = 272;

    int nStartPosX = (nScreenWidth - nWndWidth) / 2;
    int nStartPosY = (nScreenHeight - nWndHeight) / 2;

    ::MoveWindow(hWnd, nStartPosX, nStartPosY, nWndWidth, nWndHeight, TRUE);
#endif

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

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
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            CMainStation& objStation = CMainStation::getInstance();
            CWin32ScreenDevice* pDevice = objStation.GetScreenDevice();

            if (NULL != pDevice)
            {
                pDevice->DrawScreen(hdc);
            }

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

#endif
