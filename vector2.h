#pragma once
#include <sal.h>

class Vector2 {
public:
	Vector2() {
	}

	Vector2(_In_ const Vector2& v) {   // https://www.runoob.com/w3cnote/cpp-const-keyword.html
		mX = v.mX;
		mY = v.mY;
	}

	Vector2(_In_ const float x, _In_ const float y) {
		mX = x;
		mY = y;
	}

	float GetX() {
		return mX;
	}

	float GetY() {
		return mY;
	}

	void Add(_In_ const Vector2& v, _Out_ Vector2* out) {
		out->mX = mX + v.mX;
		out->mY = mY + v.mY;
	}

	Vector2& Add_(_In_ const Vector2& v) {
		mX = mX + v.mX;
		mY = mY + v.mY;
		return *this;  // https://blog.csdn.net/daimous/article/details/78618432
	}

	void Sub(_In_ const Vector2& v, _Out_ Vector2* out) {
		out->mX = mX - v.mX;
		out->mY = mY - v.mY;
	}

	Vector2& Sub_(_In_ const Vector2& v) {
		mX = mX - v.mX;
		mY = mY - v.mY;
		return *this;
	}

	void Scale(_In_ const float v, _Out_ Vector2* out) {
		out->mX = mX * v;
		out->mY = mY * v;
	}

	Vector2& Scale_(_In_ const float v) {
		mX = mX * v;
		mY = mY * v;
		return *this;
	}

	void Multi(_In_ const Vector2& v, _Out_ Vector2* out) {
		out->mX = mX * v.mX;
		out->mY = mY * v.mY;
	}

	Vector2& Multi_(_In_ const Vector2& v) {
		mX = mX * v.mX;
		mY = mY * v.mY;
		return *this;
	}

	void Div(_In_ const Vector2& v, _Out_ Vector2* out) {
		out->mX = mX / v.mX;
		out->mY = mY / v.mY;
	}

	Vector2& Div_(_In_ const Vector2& v) {
		mX = mX / v.mX;
		mY = mY / v.mY;
		return *this;
	}

	Vector2& Copy_(_In_ const Vector2& v) {
		mX = v.mX;
		mY = v.mY;
		return *this;
	}

protected:
	float mX = 0.;
	float mY = 0.;
};
