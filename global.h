#pragma once
#include <windows.h>

#define MAX_LOADSTRING 100   // ��ַ���

// ȫ�ֱ���:
ULONG_PTR gdiplusToken;	                        // GDI+���
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
