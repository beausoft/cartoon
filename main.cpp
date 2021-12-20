#include "global.h"
#include "resource.h"
#include "explorer.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

// 函数的前向声明
ATOM MainRegisterClass(HINSTANCE hInstance);  // 注册主窗口
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 窗口消息处理
BOOL InitInstance(HINSTANCE, int);   // 初始化实例
void OnPaint(_In_ HWND, _In_ HDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);
    MainRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));
    MSG msg;
    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MainRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APP));

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        if (!applyDesktop(hWnd)) {
            MessageBox(hWnd, L"设置壁纸窗口失败！", L"错误", MB_OK | MB_ICONERROR);
            // CloseWindow(hWnd); 
            DestroyWindow(hWnd);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OnPaint(hWnd, hdc);
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
    if (::GdiplusStartup(&gdiplusToken, &GpInput, NULL)) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

bool flag = false;
void OnPaint(_In_ HWND hWnd, _In_ HDC hdc) {
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

    // https://docs.microsoft.com/en-us/previous-versions/visualstudio/foxpro/ms971547(v=vs.80)
    //::LinearGradientBrush brush(NULL, NULL, Color);
}
