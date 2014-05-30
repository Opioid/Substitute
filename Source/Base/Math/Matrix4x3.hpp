#pragma once

template<typename T> struct Matrix4x4;

template<typename T> 
struct Matrix4x3
{
	union
	{
		struct
		{
			T m00, m01, m02,
			  m10, m11, m12,
			  m20, m21, m22,
			  m30, m31, m32;
		};

		T m[12];
	};

	Matrix4x3();

	Matrix4x3(T m00, T m01, T m02,
			  T m10, T m11, T m12,
			  T m20, T m21, T m22,
			  T m30, T m31, T m32);
		
	Matrix4x3(const T m[12]);

	explicit Matrix4x3(const Matrix4x4<T>& m);

	static const Matrix4x3 identity;
};
