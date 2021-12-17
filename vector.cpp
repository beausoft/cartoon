#include "vector.h"
#include <windows.h>

Vector2D::Vector2D()
{
	this->init();
}

Vector2D::~Vector2D()
{
	OutputDebugString(L"对象析构执行\n");
}

void Vector2D::init() {
	OutputDebugString(L"对象初始化\n");
	this->m1 = (char*)TEXT("sacsacascas");
}

