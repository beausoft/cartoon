#include "desktop.h"
#include <Psapi.h>
#include <string>
#include <algorithm>
using namespace std;

typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

BOOL belongToExplorer(_In_ HWND hWnd) {
    DWORD dwPid;
    if (GetWindowThreadProcessId(hWnd, &dwPid)) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
        if (NULL != hProcess) {
            WCHAR fileFullName[256];
            if (GetProcessImageFileName(hProcess, fileFullName, 256)) {
                wstring fileFullNameString(fileFullName);
                string::size_type spInd = fileFullNameString.find_last_of('\\', fileFullNameString.length());
                if (string::npos != spInd) {
                    wstring fileNameString = fileFullNameString.substr(spInd + 1, fileFullNameString.length());
                    transform(fileNameString.begin(), fileNameString.end(), fileNameString.begin(), ::tolower);
                    if (fileNameString == L"explorer.exe") {
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

BOOL CALLBACK EnumProgmanWindowsProc(_In_ HWND hWnd, _In_ LPARAM lParam)
{
    HWND hProgman = FindWindowEx(NULL, hWnd, L"Progman", NULL);
    if (NULL != hProgman) {
        if (belongToExplorer(hProgman)) {
            LPWNDINFO pInfo = (LPWNDINFO)lParam;
            pInfo->hWnd = hProgman;
            return FALSE;
        }
    }
    return TRUE;
}

HWND findProgman() {
	WNDINFO wi = { 0 };
	EnumWindows(EnumProgmanWindowsProc, (LPARAM)&wi);
	return wi.hWnd;
}

BOOL CALLBACK EnumWorkerWindowsProc(_In_ HWND hWnd, _In_ LPARAM lParam)
{
    HWND hDefView = FindWindowEx(hWnd, 0, L"SHELLDLL_DefView", 0);
    if (hDefView != 0) {
        // 找它的下一个窗口，类名为WorkerW，隐藏它
        HWND hWorkerw = FindWindowEx(0, hWnd, L"WorkerW", 0);
        if (NULL != hWorkerw) {
            if (belongToExplorer(hWorkerw)) {
                LPWNDINFO pInfo = (LPWNDINFO)lParam;
                pInfo->hWnd = hWorkerw;
                return FALSE;
            }
        }
    }
    return TRUE;
}

HWND findWorkerW() {
    WNDINFO wi = { 0 };
    EnumWindows(EnumWorkerWindowsProc, (LPARAM)&wi);
    return wi.hWnd;
}

BOOL SetBackground(_In_ HWND hWnd) {
    HWND hProgman = findProgman();
    if (NULL == hProgman) {
        return FALSE;
    }
    if (hWnd) {
        DWORD_PTR result = NULL;
        SendMessageTimeout(hProgman, 0x52C, 0, 0, SMTO_NORMAL, 2000, &result);	// 给它发特殊消息
        HWND hWorkerw = findWorkerW();
        if (NULL == hWorkerw) {
            return FALSE;
        }
        // 看了其他壁纸引擎，它们都是将窗口放在hWorkerw窗口下。
        SetParent(hWnd, hWorkerw);      // 设置父窗口
        return TRUE;
    }
    else {
        /*
        大佬（https://space.bilibili.com/160424952）原话：
        关于这个玩意吧，虽然理论上 Windows 7 （Aero模式）到 Windows 10都是支持 0x52C 这个操作的，但是还是有些细节要注意一下
        首先是不同版本系统上表现不太一样，然后 Windows 10 上有的版本行不通，这时候可以试试在设置里面将壁纸设置成“幻灯片放映”模式再试试，因为 WorkerW 的存在本来是用作 Windows 桌面壁纸切换时候过渡用的。。。所以其实开启“幻灯片放映”时会有可能它自己变成两个 WorkerW 的样子而不用 0x52C 消息。。。
        然后基于上一条，因为隐藏了留存静态壁纸的 WorkerW 所以在动态壁纸直接关闭后，如果原始的桌面壁纸是“幻灯片放映”模式的话，由于 WorkerW 还是处于隐藏状态，有一定概率会导致静态壁纸切换时失去渐变过渡的效果
        再基于上一条扩展一下，由于只是隐藏了 WorkerW 而不是淦掉了它所以，在动态壁纸切播放期间产生了静态壁纸的切换过渡的话，会有一定概率覆盖动态壁纸，不过过渡动画播放完后还会继续显示动态壁纸就是了
        冷知识：这种方法实现的动态壁纸并不会响应鼠标和键盘事件，所以想完花活或仿制小红车的话，还要自己写个消息循环和消息处理
        然后友情提示一下，可以通过将发送消息时 0x52C 后面一个参数从 0 改成 1 再发送一次消息来还原为之前的状态，前提是没有手动销毁过 WorkerW 窗口并且你的桌面壁纸原本是一张图片而不是“幻灯片放映”模式。
        如果原本是“幻灯片放映”模式的话会因为这个操作将 WorkerW 窗口被关掉了从而失去静态壁纸的过渡动画，虽然还能再发 0x52C 重新激活 WorkerW 窗口就是了。
        虽然用 0x52C 消息关闭 WorkerW 窗口通常并没有什么必要，但是却可以利用这种方式来关闭 WorkerW 从而使得原本的桌面壁纸是“幻灯片放映”模式时，会跳出壁纸切换过渡动画挡住动态壁纸的问题，但是请记得还原它。
        */
        DWORD_PTR result = NULL;
        SendMessageTimeout(hProgman, 0x52C, 1, 0, SMTO_NORMAL, 2000, &result);	// 给它发特殊消息，还原之前状态
        return TRUE;
    }
}

#define WM_SETWALLPAPER WM_USER + 300    // https://blog.csdn.net/weixin_43820461/article/details/111054558
BOOL RefreshBackground() {
    HWND hWndShell = GetShellWindow();
    if (NULL == hWndShell) {
        return FALSE;
    }
    // wParam	未知，似乎可以指定任意值，系统使用0x0D
    // lParam	指定窗口是否重绘
    // PostMessage(GetShellWindow(), WM_SETWALLPAPER, 0x0D, TRUE);
    DWORD_PTR result = NULL;
    SendMessageTimeout(hWndShell, WM_SETWALLPAPER, 0x0D, TRUE, SMTO_NORMAL, 2000, &result);
    return TRUE;
}

/*
检测是否有程序处于全屏状态了，灵感来自QQ的繁忙模式切换！
*/
BOOL TestFullScreen() {
    HWND hWndForegroundWindow = GetForegroundWindow();   // 获取前置窗口
    if (hWndForegroundWindow) {
        DWORD dwPid;
        GetWindowThreadProcessId(hWndForegroundWindow, &dwPid);
        if (dwPid == GetCurrentProcessId()) {
            // 如果是当前进程的窗口则直接返回
            return FALSE;
        }
        if (belongToExplorer(hWndForegroundWindow)) {
            // 如果是explorer则直接返回
            return FALSE;
        }
        RECT foregroundWindowRect;
        if (GetWindowRect(hWndForegroundWindow, &foregroundWindowRect)) {
            // 获取但不包括任务栏的屏幕尺寸
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            // 计算屏幕面积和前置窗口面积
            int screenAcreage = screenWidth * screenHeight;
            int foregroundAcreage = (foregroundWindowRect.right - foregroundWindowRect.left) * (foregroundWindowRect.bottom - foregroundWindowRect.top);

            float scale = float(foregroundAcreage) / float(screenAcreage);
            // 当前置窗口的占有率大于95%时判定为全屏
            if (scale > 0.95f) {
                return TRUE;
            }
        }
    }
    return FALSE;
}
