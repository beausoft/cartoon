#pragma once
#include <windows.h>

#define MAX_LOADSTRING 100   // ��ַ���
#define TIMERID_REDRAW 0x21  // �ػ涨ʱ��ID

// ȫ�ֱ���:
ULONG_PTR gdiplusToken;	                        // GDI+���
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
