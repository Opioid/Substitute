#pragma once

#include "Matrix4x3.hpp"

template<typename T>
inline Matrix4x3<T>::Matrix4x3()
{}

template<typename T>
inline Matrix4x3<T>::Matrix4x3(T m00, T m01, T m02,
							   T m10, T m11, T m12,
							   T m20, T m21, T m22,
							   T m30, T m31, T m32) :
	m00(m00), m01(m01), m02(m02),
	m10(m10), m11(m11), m12(m12),
	m20(m20), m21(m21), m22(m22),
	m30(m30), m31(m31), m32(m32)
{}

template<typename T>
inline Matrix4x3<T>::Matrix4x3(const T m[12])
{
	m00 = m[0]; m01 = m[1];  m02 = m[2];
	m10 = m[3]; m11 = m[4];  m12 = m[5];
	m20 = m[6]; m21 = m[7];  m22 = m[8];
	m30 = m[9]; m31 = m[10]; m32 = m[11];
}

template<typename T>
inline Matrix4x3<T>::Matrix4x3(const Matrix4x4<T>& m) :
	m00(m.m00), m01(m.m01), m02(m.m02),
	m10(m.m10), m11(m.m11), m12(m.m12),
	m20(m.m20), m21(m.m21), m22(m.m22),
	m30(m.m30), m31(m.m31), m32(m.m32)
{}

template<typename T>
const Matrix4x3<T> Matrix4x3<T>::identity = Matrix4x3<T>(T(1), T(0), T(0),
														 T(0), T(1), T(0),
														 T(0), T(0), T(1),
														 T(0), T(0), T(0));
