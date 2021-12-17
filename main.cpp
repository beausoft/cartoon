#include "resource.h"
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#include "vector.h"
#include <string>
#include <math.h>
using namespace std;


#define MAX_LOADSTRING 100

// 全局变量:
ULONG_PTR token;	                            //GDI+句柄
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名


// 此代码模块中包含的函数的前向声明:
// ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        /*
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        */
    }

    return (int) msg.wParam;
}





BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd) {
      return FALSE;
   }

   // 初始化GDI+
   ::GdiplusStartupInput GpInput;
   GpInput.GdiplusVersion = 1;
   if (::GdiplusStartup(&token, &GpInput, NULL)) {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

bool flag = false;

void OnPaint(HDC hdc) {
    ::Graphics graphics(hdc);
    Pen          pen(Color(255, 0, 0, 255));
    FontFamily   fontFamily(L"宋体");
    Font         font(&fontFamily, 12, FontStyleRegular, UnitPixel);
    PointF       pointF1(30.0f, 60.0f), pointF2(230.0f, 60.0f);
    SolidBrush   solidBrush(Color(255, 0, 0, 255));
    StringFormat stringFormat;
    WCHAR testString[] = L"Hello034∠你好";

    Color backFullColor;
    if (flag) {
        flag = false;
        backFullColor = Color(255, 255, 0, 255);
    }
    else {
        flag = true;
        backFullColor = Color(255, 0, 255, 255);
    }
    graphics.Clear(backFullColor);

    stringFormat.SetFormatFlags(StringFormatFlagsDirectionVertical);

    graphics.SetSmoothingMode(SmoothingModeDefault);
    graphics.DrawLine(&pen, 0, 0, 200, 100);
    graphics.DrawEllipse(&pen, 10, 10, 190, 90);
    graphics.SetTextRenderingHint(TextRenderingHintSystemDefault);
    graphics.DrawString(testString, -1, &font, pointF1, &stringFormat, &solidBrush);


    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    graphics.DrawLine(&pen, 200, 0, 400, 100);
    graphics.DrawEllipse(&pen, 210, 10, 190, 90);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.DrawString(testString, -1, &font, pointF2, &stringFormat, &solidBrush);

    //graphics.DrawCurve()

    OutputDebugString(TEXT("渲染\n"));

    Vector2D vector;
    OutputDebugString(to_wstring((int)vector.m1).data());
   
    // https://docs.microsoft.com/en-us/previous-versions/visualstudio/foxpro/ms971547(v=vs.80)
    //::LinearGradientBrush brush(NULL, NULL, Color);
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam)
{
    HWND hDefView = FindWindowEx(hwnd, 0, L"SHELLDLL_DefView", 0);
    if (hDefView != 0) {
        // 找它的下一个窗口，类名为WorkerW，隐藏它
        HWND hWorkerw = FindWindowEx(0, hwnd, L"WorkerW", 0);
        ShowWindow(hWorkerw, SW_HIDE);

        return FALSE;
    }
    return TRUE;
}

void toDesktop(_In_ HWND hWnd) {
    HWND hProgman = FindWindow(L"Progman", NULL);  // 找到PM窗口
    SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 100, 0);	// 给它发特殊消息
    SetParent(hWnd, hProgman);
    EnumWindows(EnumWindowsProc, 0);
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
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
            case 112:
            {
                // MessageBox(hWnd, L"对话框内容", L"标题", MB_OK);
                // SendMessage(hWnd, WM_PAINT, NULL, NULL);
                RECT invalidateRect = { 10, 10, 100, 100 };
                InvalidateRect(hWnd, &invalidateRect, FALSE);
                UpdateWindow(hWnd);
            }
            break;
            case 113:
            {
                /*RECT rect = { 0 };
                GetWindowRect(hWnd, &rect);
                rect.right = rect.right - rect.left;
                rect.bottom = rect.bottom - rect.top;
                rect.left = 0;
                rect.top = 0;*/
                InvalidateRect(hWnd, NULL, FALSE);
                UpdateWindow(hWnd);
            }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            OnPaint(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        OutputDebugString(TEXT("窗口创建\n"));
        CreateWindow(TEXT("Button"), TEXT("测试"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 300, 200, 120, 60, hWnd, (HMENU)112, hInst, NULL);
        CreateWindow(TEXT("Button"), TEXT("测试1"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 380, 330, 120, 60, hWnd, (HMENU)113, hInst, NULL);
        // toDesktop(hWnd);
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
