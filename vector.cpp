#include "vector.h"
#include <windows.h>

Vector2D::Vector2D()
{
	this->init();
}

Vector2D::~Vector2D()
{
	OutputDebugString(L"��������ִ��\n");
}

void Vector2D::init() {
	OutputDebugString(L"�����ʼ��\n");
	this->m1 = (char*)TEXT("sacsacascas");
}

