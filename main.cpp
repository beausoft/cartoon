#include "global.h"
#include "resource.h"
#include "desktop.h"
#include "vector2.h"
#include "eye.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#include <cstdlib>

// 函数的前向声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 窗口消息处理
void OnPaint(_In_ HWND, _In_ HDC);

AnimationSchedule schedule(300);  // 调度服务
Vector2 AP1(EYE_AP1);
Vector2 AP2(EYE_AP2);
Vector2 BP1(EYE_BP1);
Vector2 BP2(EYE_BP2);
BOOL isKernel = TRUE;
HWND hWndMain = NULL;
// 通知托盘
#define WM_TRAY_NOTIFY WM_USER + 1000
#define IDM_EXIT 1005
NOTIFYICONDATA trayNotify;
HMENU hMenuTrayNotify;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);

    HANDLE hMutex = CreateMutex(NULL, TRUE, L"WallpaperCartoonEyeMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // https://www.cnblogs.com/duguxue/p/5231371.html
        if (hMutex != NULL) {
            CloseHandle(hMutex); //互斥量存在释放句柄并复位互斥量
        }
        return FALSE;
    }

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

    int cw = 0, ch = 0;
    {
        // https://blog.csdn.net/yp18792574062/article/details/88351342
        HDC hdc = GetDC(NULL);
        cw = GetDeviceCaps(hdc, DESKTOPHORZRES);
        ch = GetDeviceCaps(hdc, DESKTOPVERTRES);
        ReleaseDC(NULL, hdc);
    }

    hInst = hInstance; // 将实例句柄存储在全局变量中
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_CHILD | WS_POPUP | WS_EX_TOOLWINDOW, 0, 0, cw, ch, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        MessageBeep(MB_ICONERROR);
        return FALSE;
    }
    hWndMain = hWnd;

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

void openEye(float alpha, bool running) {
    if (running) {
        AP1.Copy_(EYE_AP1.Sub_(EYE_AP1_TO).Scale_(alpha).Add_(EYE_AP1_TO));
        AP2.Copy_(EYE_AP2.Sub_(EYE_AP2_TO).Scale_(alpha).Add_(EYE_AP2_TO));
        BP1.Copy_(EYE_BP1.Sub_(EYE_BP1_TO).Scale_(alpha).Add_(EYE_BP1_TO));
        BP2.Copy_(EYE_BP2.Sub_(EYE_BP2_TO).Scale_(alpha).Add_(EYE_BP2_TO));
        if (alpha == 1.0f) {
            isKernel = TRUE;
        }
    }
    
}

void closeEye(float alpha, bool running) {
    if (running) {
        AP1.Copy_(EYE_AP1_TO.Sub_(EYE_AP1).Scale_(alpha).Add_(EYE_AP1));
        AP2.Copy_(EYE_AP2_TO.Sub_(EYE_AP2).Scale_(alpha).Add_(EYE_AP2));
        BP1.Copy_(EYE_BP1_TO.Sub_(EYE_BP1).Scale_(alpha).Add_(EYE_BP1));
        BP2.Copy_(EYE_BP2_TO.Sub_(EYE_BP2).Scale_(alpha).Add_(EYE_BP2));
        if (alpha == 1.0f) {
            isKernel = FALSE;
        }
    }
}

void lastSchedule(float alpha, bool running) {
    if (running && 1.0f == alpha) {
        KillTimer(hWndMain, TIMERID_REDRAW);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_EXIT:
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_TRAY_NOTIFY:
    {
        UINT uID = (UINT)wParam;
        UINT uMouseMsg = (UINT)lParam;
        if (uID == IDI_APP && uMouseMsg == WM_RBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd);   // 解决在菜单外单击左键菜单不消失的问题  https://www.iteye.com/blog/cwqcwk1-2289166
            TrackPopupMenu(hMenuTrayNotify, TPM_RIGHTBUTTON, pt.x, pt.y, NULL, hWnd, NULL);
        }
    }
    break;
    case WM_CREATE:
        if (!SetBackground(hWnd)) {
            MessageBox(hWnd, L"设置壁纸窗口失败！", L"错误", MB_OK | MB_ICONERROR);
            DestroyWindow(hWnd);
        } else {
            SetTimer(hWnd, TIMERID_HEARTBEAT, 3000, NULL);
            schedule.addTrack(0, 30, closeEye);
            schedule.addTrack(100, 130, openEye);
            schedule.addTrack(200, 230, closeEye);
            schedule.addTrack(230, 260, openEye);
            schedule.addTrack(300, 300, lastSchedule);
            // 初始化图片菜单
            hMenuTrayNotify = CreatePopupMenu();
            AppendMenu(hMenuTrayNotify, MF_STRING, IDM_EXIT, L"Exit(&Q)");
            // 初始化托盘
            trayNotify.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
            trayNotify.hWnd = hWnd;
            trayNotify.uID = IDI_APP;   // 此ID随便设置
            trayNotify.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
            trayNotify.uCallbackMessage = WM_TRAY_NOTIFY;  // 响应消息
            trayNotify.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APP));
            wcscpy_s(trayNotify.szTip, L"Wallpaper Cartoon EYE!");
            wcscpy_s(trayNotify.szInfoTitle, L"Wallpaper Cartoon EYE");
            wcscpy_s(trayNotify.szInfo, L"Wallpaper Cartoon eye Running!");
            Shell_NotifyIcon(NIM_ADD, &trayNotify);//在托盘区添加图标
        }
        break;
    case WM_TIMER:
    {
        int eventId = LOWORD(wParam);
        if (eventId == TIMERID_HEARTBEAT) {
            // 只有当非全屏状态下才触发动画效果
            if (!TestFullScreen()) {
                int p = std::rand() % 100 + 1;
                if (p <= 35) {   // 以35的概率执行动画
                    if (schedule.start()) {
                        SetTimer(hWnd, TIMERID_REDRAW, 17, NULL);
                    }
                }
            }
        }else if (eventId == TIMERID_REDRAW) {
            schedule.update();
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
    case WM_DISPLAYCHANGE:
    {
        // int p = int(lParam);
        int width = LOWORD(lParam);    // p & 0xFFFF;
        int height = HIWORD(lParam);   // >> 16;
        MoveWindow(hWnd, 0, 0, width, height, TRUE);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        SetParent(hWnd, NULL);
        SetBackground(NULL);
        RefreshBackground();
        // 在托盘中删除图标
        Shell_NotifyIcon(NIM_DELETE, &trayNotify);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnPaint(_In_ HWND hWnd, _In_ HDC hdc) {
    // 双缓冲 https://blog.csdn.net/weixin_33894640/article/details/94608441
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

    Color backColor(255, 0, 0, 0);
    Color frontColor(255, 255, 255, 255);
    Color kernelColor(255, 0, 0, 0);

    graphics.Clear(backColor);

    Vector2 lap0, lap1, lap2, lap3, lbp0, lbp1, lbp2, lbp3;

    float scale = height * 0.8f;
    EYE_AP0.Scale(scale, &lap0);
    AP1.Scale(scale, &lap1);
    AP2.Scale(scale, &lap2);
    EYE_AP3.Scale(scale, &lap3);
    EYE_BP0.Scale(scale, &lbp0);
    BP1.Scale(scale, &lbp1);
    BP2.Scale(scale, &lbp2);
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

    if (isKernel) {
        float kernalRadius = 0.05f * scale;

        Vector2 kernalLeft;
        EYE_KERNEL.Scale(scale, &kernalLeft);
        kernalLeft.Add_(offsetLeft);
        ::SolidBrush kernelBrush(kernelColor);
        graphics.FillEllipse(&kernelBrush, kernalLeft.GetX() - kernalRadius, kernalLeft.GetY() - kernalRadius, kernalRadius * 2, kernalRadius * 2);

        Vector2 kernalRight(lap3.GetX() - kernalLeft.GetX() + rap0.GetX(), kernalLeft.GetY());
        graphics.FillEllipse(&kernelBrush, kernalRight.GetX() - kernalRadius, kernalRight.GetY() - kernalRadius, kernalRadius * 2, kernalRadius * 2);
    }
    BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    DeleteObject(hBitmap);
}
