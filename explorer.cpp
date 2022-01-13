#include "explorer.h"
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
                    wstring fileNameString = fileFullNameString.substr(((int)spInd) + 1, fileFullNameString.length());
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

BOOL regainDesktop(_In_ HWND hWnd) {
    // TODO ��δ�о�������λָ�explorer��δ����0x52C�¼�֮ǰ��״̬��
    HWND hWorkerw = findWorkerW();
    if (NULL == hWorkerw) {
        return FALSE;
    }
    // SetParent(hWnd, NULL);      // ���ø�����
    ShowWindow(hWorkerw, SW_SHOW);  // ��ʾ����
}

BOOL applyDesktop(_In_ HWND hWnd) {
    HWND hProgman = findProgman();
    if (NULL == hProgman) {
        return FALSE;
    }
    SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 100, 0);	// ������������Ϣ
    HWND hWorkerw = findWorkerW();
    if (NULL == hWorkerw) {
        return FALSE;
    }
    SetParent(hWnd, hProgman);      // ���ø�����
    ShowWindow(hWorkerw, SW_HIDE);  // ���ش���
	return TRUE;
}
