#include "global.h"
#include "resource.h"
#include "explorer.h"
#include "vector2.h"
#include "eye.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

// 函数的前向声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 窗口消息处理
void OnPaint(_In_ HWND, _In_ HDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);

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
    if (!RegisterClassExW(&wcex)) {  // 窗口类的注册
        MessageBeep(MB_ICONERROR);
        return FALSE;
    }

    hInst = hInstance; // 将实例句柄存储在全局变量中
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        MessageBeep(MB_ICONERROR);
        return FALSE;
    }

    // 初始化GDI+
    ::GdiplusStartupInput GpInput;
    GpInput.GdiplusVersion = 1;
    if (::GdiplusStartup(&gdiplusToken, &GpInput, NULL)) {
        MessageBeep(MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    // 释放GDI+
    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case 0: // 防止警告
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
        /*
        if (!applyDesktop(hWnd)) {
            MessageBox(hWnd, L"设置壁纸窗口失败！", L"错误", MB_OK | MB_ICONERROR);
            DestroyWindow(hWnd);
        }*/
        SetTimer(hWnd, TIMERID_REDRAW, 17, NULL);
        break;
    case WM_TIMER:
    {
        int eventId = LOWORD(wParam);
        if (eventId == TIMERID_REDRAW) {
            HDC hdc = GetDC(hWnd);
            OnPaint(hWnd, hdc);
            ReleaseDC(hWnd, hdc);
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
    case WM_CLOSE:
        regainDesktop(hWnd);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnPaint(_In_ HWND hWnd, _In_ HDC hdc) {
    // https://blog.csdn.net/weixin_33894640/article/details/94608441 双缓冲
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(hMemDC, hBitmap);
    ::Graphics graphics(hMemDC);   // https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-graphics-flat
    graphics.SetSmoothingMode(::SmoothingMode::SmoothingModeHighQuality);
    graphics.SetInterpolationMode(::InterpolationMode::InterpolationModeBilinear);

    Color backColor(0, 0, 0, 255);
    Color frontColor(255, 255, 255, 255);
    Color kernelColor(0, 0, 0, 255);

    graphics.Clear(backColor);

    Vector2 lap0, lap1, lap2, lap3, lbp0, lbp1, lbp2, lbp3;

    float scale = height * 0.8f;
    EYE_AP0.Scale(scale, &lap0);
    EYE_AP1.Scale(scale, &lap1);
    EYE_AP2.Scale(scale, &lap2);
    EYE_AP3.Scale(scale, &lap3);
    EYE_BP0.Scale(scale, &lbp0);
    EYE_BP1.Scale(scale, &lbp1);
    EYE_BP2.Scale(scale, &lbp2);
    EYE_BP3.Scale(scale, &lbp3);

    ::SolidBrush frontBrush(frontColor);

    Vector2 offsetLeft(width / 2.0f - lap3.GetX() - (width / 10.0f), (height - scale) / 2.0f);
    lap0.Add_(offsetLeft);
    lap1.Add_(offsetLeft);
    lap2.Add_(offsetLeft);
    lap3.Add_(offsetLeft);
    lbp0.Add_(offsetLeft);
    lbp1.Add_(offsetLeft);
    lbp2.Add_(offsetLeft);
    lbp3.Add_(offsetLeft);

    ::GraphicsPath leftPath;   // https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-graphicspath-flat
    PointF point1(lap0.GetX(), lap0.GetY());
    PointF point2(lap1.GetX(), lap1.GetY());
    PointF point3(lap2.GetX(), lap2.GetY());
    PointF point4(lap3.GetX(), lap3.GetY());
    leftPath.AddBezier(point1, point2, point3, point4);
    PointF point5(lbp0.GetX(), lbp0.GetY());
    PointF point6(lbp1.GetX(), lbp1.GetY());
    PointF point7(lbp2.GetX(), lbp2.GetY());
    PointF point8(lbp3.GetX(), lbp3.GetY());
    leftPath.AddBezier(point5, point6, point7, point8);
    graphics.FillPath(&frontBrush, &leftPath);

    Vector2 rap0(lap0.GetX(), lap3.GetY());
    Vector2 rap1(lap1.GetX(), lap2.GetY());
    Vector2 rap2(lap2.GetX(), lap1.GetY());
    Vector2 rap3(lap3.GetX(), lap0.GetY());
    Vector2 rbp0(lbp0.GetX(), lbp3.GetY());
    Vector2 rbp1(lbp1.GetX(), lbp2.GetY());
    Vector2 rbp2(lbp2.GetX(), lbp1.GetY());
    Vector2 rbp3(lbp3.GetX(), lbp0.GetY());

    Vector2 offsetRight(width / 2.0f + width / 10.0f, offsetLeft.GetY());
    rap0.Sub_(offsetLeft).Add_(offsetRight);
    rap1.Sub_(offsetLeft).Add_(offsetRight);
    rap2.Sub_(offsetLeft).Add_(offsetRight);
    rap3.Sub_(offsetLeft).Add_(offsetRight);
    rbp0.Sub_(offsetLeft).Add_(offsetRight);
    rbp1.Sub_(offsetLeft).Add_(offsetRight);
    rbp2.Sub_(offsetLeft).Add_(offsetRight);
    rbp3.Sub_(offsetLeft).Add_(offsetRight);

    ::GraphicsPath rightPath;
    PointF point9(rap0.GetX(), rap0.GetY());
    PointF point10(rap1.GetX(), rap1.GetY());
    PointF point11(rap2.GetX(), rap2.GetY());
    PointF point12(rap3.GetX(), rap3.GetY());
    rightPath.AddBezier(point9, point10, point11, point12);
    PointF point13(rbp0.GetX(), rbp0.GetY());
    PointF point14(rbp1.GetX(), rbp1.GetY());
    PointF point15(rbp2.GetX(), rbp2.GetY());
    PointF point16(rbp3.GetX(), rbp3.GetY());
    rightPath.AddBezier(point13, point14, point15, point16);
    graphics.FillPath(&frontBrush, &rightPath);


    
    
    

    /*
    Vector2 ap0(EYE_AP0);
    Vector2 ap1(EYE_AP1);
    Vector2 ap2(EYE_AP2);
    Vector2 ap3(EYE_AP3);
    Vector2 bp0(EYE_BP0);
    Vector2 bp1(EYE_BP1);
    Vector2 bp2(EYE_BP2);
    Vector2 bp3(EYE_BP3);

    double scale = height * 0.8;
    ap0.Scale_(scale);
    ap1.Scale_(scale);
    ap2.Scale_(scale);
    ap3.Scale_(scale);
    bp0.Scale_(scale);
    bp1.Scale_(scale);
    bp2.Scale_(scale);
    bp3.Scale_(scale);*/
    
    
    /*Vector2 offset(width / 2)* /




    // graphics.DrawBeziers(Pen(Color::Green, 3), )

    /*
    SolidBrush blackBrush(Color(128, 0, 0, 255));
    PointF point1(100.0f, 100.0f);
    PointF point2(200.0f, 50.0f);
    PointF point3(250.0f, 200.0f);
    PointF point4(50.0f, 150.0f);
    PointF points[4] = { point1, point2, point3, point4 };

    //填充闭合区域  
    graphics.FillClosedCurve(&blackBrush, points, 4);
    //为闭合区域画边框  
    Pen curPen(Color::Green, 3);
    graphics.DrawClosedCurve(&curPen, points, 4);
    */

    /*
    Vector2 p1(10., 10.);
    Vector2 p2(20., 20.);
    Vector2 v3(p2); // p1.Add(p2, &Vector2());

    double a = v3.Add_(p1).Add_(p2).Scale_(1. / 3. ).Copy_(p1).Add_(p2).Add_(EYE_AP0).GetX();

    ::Graphics graphics(hMemDC);
    Pen          pen(Color(255, 0, 0, 255));
    FontFamily   fontFamily(L"宋体");
    Font         font(&fontFamily, 12, FontStyleRegular, UnitPixel);
    PointF       pointF1(30.0f, 60.0f), pointF2(230.0f, 60.0f);
    SolidBrush   solidBrush(Color(255, 0, 0, 255));
    StringFormat stringFormat;
    WCHAR testString[] = L"Hello034∠你好";

    Color backFullColor(0, 0, 0, 255);
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
    */
    BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    DeleteObject(hBitmap);
}
