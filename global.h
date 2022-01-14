#pragma once
#include <windows.h>

#define MAX_LOADSTRING 100   // 最长字符串
#define TIMERID_REDRAW 0x21  // 重绘定时器ID
#define TIMERID_HEARTBEAT 0x22  // 触发定时器ID

// 全局变量:
ULONG_PTR gdiplusToken;	                        // GDI+句柄
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
