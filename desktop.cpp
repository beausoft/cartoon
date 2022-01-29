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
        // ��������һ�����ڣ�����ΪWorkerW��������
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
        SendMessageTimeout(hProgman, 0x52C, 0, 0, SMTO_NORMAL, 2000, &result);	// ������������Ϣ
        HWND hWorkerw = findWorkerW();
        if (NULL == hWorkerw) {
            return FALSE;
        }
        // ����������ֽ���棬���Ƕ��ǽ����ڷ���hWorkerw�����¡�
        SetParent(hWnd, hWorkerw);      // ���ø�����
        return TRUE;
    }
    else {
        /*
        ���У�https://space.bilibili.com/160424952��ԭ����
        �����������ɣ���Ȼ������ Windows 7 ��Aeroģʽ���� Windows 10����֧�� 0x52C ��������ģ����ǻ�����Щϸ��Ҫע��һ��
        �����ǲ�ͬ�汾ϵͳ�ϱ��ֲ�̫һ����Ȼ�� Windows 10 ���еİ汾�в�ͨ����ʱ������������������潫��ֽ���óɡ��õ�Ƭ��ӳ��ģʽ�����ԣ���Ϊ WorkerW �Ĵ��ڱ��������� Windows �����ֽ�л�ʱ������õġ�����������ʵ�������õ�Ƭ��ӳ��ʱ���п������Լ�������� WorkerW �����Ӷ����� 0x52C ��Ϣ������
        Ȼ�������һ������Ϊ���������澲̬��ֽ�� WorkerW �����ڶ�̬��ֱֽ�ӹرպ����ԭʼ�������ֽ�ǡ��õ�Ƭ��ӳ��ģʽ�Ļ������� WorkerW ���Ǵ�������״̬����һ�����ʻᵼ�¾�̬��ֽ�л�ʱʧȥ������ɵ�Ч��
        �ٻ�����һ����չһ�£�����ֻ�������� WorkerW �������Ƶ��������ԣ��ڶ�̬��ֽ�в����ڼ�����˾�̬��ֽ���л����ɵĻ�������һ�����ʸ��Ƕ�̬��ֽ���������ɶ���������󻹻������ʾ��̬��ֽ������
        ��֪ʶ�����ַ���ʵ�ֵĶ�̬��ֽ��������Ӧ���ͼ����¼����������껨������С�쳵�Ļ�����Ҫ�Լ�д����Ϣѭ������Ϣ����
        Ȼ��������ʾһ�£�����ͨ����������Ϣʱ 0x52C ����һ�������� 0 �ĳ� 1 �ٷ���һ����Ϣ����ԭΪ֮ǰ��״̬��ǰ����û���ֶ����ٹ� WorkerW ���ڲ�����������ֽԭ����һ��ͼƬ�����ǡ��õ�Ƭ��ӳ��ģʽ��
        ���ԭ���ǡ��õ�Ƭ��ӳ��ģʽ�Ļ�����Ϊ��������� WorkerW ���ڱ��ص��˴Ӷ�ʧȥ��̬��ֽ�Ĺ��ɶ�������Ȼ�����ٷ� 0x52C ���¼��� WorkerW ���ھ����ˡ�
        ��Ȼ�� 0x52C ��Ϣ�ر� WorkerW ����ͨ����û��ʲô��Ҫ������ȴ�����������ַ�ʽ���ر� WorkerW �Ӷ�ʹ��ԭ���������ֽ�ǡ��õ�Ƭ��ӳ��ģʽʱ����������ֽ�л����ɶ�����ס��̬��ֽ�����⣬������ǵû�ԭ����
        */
        DWORD_PTR result = NULL;
        SendMessageTimeout(hProgman, 0x52C, 1, 0, SMTO_NORMAL, 2000, &result);	// ������������Ϣ����ԭ֮ǰ״̬
        return TRUE;
    }
}

#define WM_SETWALLPAPER WM_USER + 300    // https://blog.csdn.net/weixin_43820461/article/details/111054558
BOOL RefreshBackground() {
    HWND hWndShell = GetShellWindow();
    if (NULL == hWndShell) {
        return FALSE;
    }
    // wParam	δ֪���ƺ�����ָ������ֵ��ϵͳʹ��0x0D
    // lParam	ָ�������Ƿ��ػ�
    // PostMessage(GetShellWindow(), WM_SETWALLPAPER, 0x0D, TRUE);
    DWORD_PTR result = NULL;
    SendMessageTimeout(hWndShell, WM_SETWALLPAPER, 0x0D, TRUE, SMTO_NORMAL, 2000, &result);
    return TRUE;
}

/*
����Ƿ��г�����ȫ��״̬�ˣ��������QQ�ķ�æģʽ�л���
*/
BOOL TestFullScreen() {
    HWND hWndForegroundWindow = GetForegroundWindow();   // ��ȡǰ�ô���
    if (hWndForegroundWindow) {
        DWORD dwPid;
        GetWindowThreadProcessId(hWndForegroundWindow, &dwPid);
        if (dwPid == GetCurrentProcessId()) {
            // ����ǵ�ǰ���̵Ĵ�����ֱ�ӷ���
            return FALSE;
        }
        if (belongToExplorer(hWndForegroundWindow)) {
            // �����explorer��ֱ�ӷ���
            return FALSE;
        }
        RECT foregroundWindowRect;
        if (GetWindowRect(hWndForegroundWindow, &foregroundWindowRect)) {
            // ��ȡ������������������Ļ�ߴ�
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            // ������Ļ�����ǰ�ô������
            int screenAcreage = screenWidth * screenHeight;
            int foregroundAcreage = (foregroundWindowRect.right - foregroundWindowRect.left) * (foregroundWindowRect.bottom - foregroundWindowRect.top);

            float scale = float(foregroundAcreage) / float(screenAcreage);
            // ��ǰ�ô��ڵ�ռ���ʴ���95%ʱ�ж�Ϊȫ��
            if (scale > 0.95f) {
                return TRUE;
            }
        }
    }
    return FALSE;
}
