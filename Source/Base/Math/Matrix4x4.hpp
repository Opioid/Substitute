#pragma once

#include "Matrix3x3.hpp"
#include "Vector4.hpp"
#include "Math.hpp"

struct Plane;

template<typename T> 
struct tMatrix4x4
{
	/*__declspec(align(16))*/ union
	{
		struct
		{
			T m00, m01, m02, m03,
			  m10, m11, m12, m13,
			  m20, m21, m22, m23,
			  m30, m31, m32, m33;
		};

		T m[16];

		struct 
		{
			tVector4<T> rows[4];
		};
	};

	tMatrix4x4()
	{}

	tMatrix4x4(T m00, T m01, T m02, T m03,
	           T m10, T m11, T m12, T m13,
	           T m20, T m21, T m22, T m23,
			   T m30, T m31, T m32, T m33) :
		m00(m00), m01(m01), m02(m02), m03(m03),
		m10(m10), m11(m11), m12(m12), m13(m13),
		m20(m20), m21(m21), m22(m22), m23(m23),
		m30(m30), m31(m31), m32(m32), m33(m33)
	{}
		
	tMatrix4x4(const T m[16])
	{
		m00 = m[0];  m01 = m[1];  m02 = m[2];  m03 = m[3];
		m10 = m[4];  m11 = m[5];  m12 = m[6];  m13 = m[7];
		m20 = m[8];  m21 = m[9];  m22 = m[10]; m23 = m[11];
		m30 = m[12]; m31 = m[13]; m32 = m[14]; m33 = m[15];
	}

	explicit tMatrix4x4(const tMatrix3x3<T> &m) :
		m00(m.m00), m01(m.m01), m02(m.m02), m03(T(0)),
		m10(m.m10), m11(m.m11), m12(m.m12), m13(T(0)),
		m20(m.m20), m21(m.m21), m22(m.m22), m23(T(0)),
		m30(T(0)),  m31(T(0)),  m32(T(0)),  m33(T(1))
	{}

	tMatrix4x4 operator*(const tMatrix4x4 &m) const
	{
		return tMatrix4x4(m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30,
						  m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31,
						  m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32,
						  m00 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33,

						  m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30,
						  m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31,
						  m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32,
						  m10 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33,

						  m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30,
						  m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31,
						  m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32,
						  m20 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33,

						  m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30,
						  m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31,
						  m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32,
						  m30 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33);
	}

	tMatrix4x4 operator/(T s) const
	{
		T is = T(1) / s;
		return tMatrix4x4(is * m00, is * m01, is * m02, is * m03,
						   is * m10, is * m11, is * m12, is * m13,
						   is * m20, is * m21, is * m22, is * m23,
						   is * m30, is * m31, is * m32, is * m33);
	}

	tMatrix4x4 &operator*=(const tMatrix4x4 &m)
	{
		tMatrix4x4 tmp(m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30,
						m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31,
						m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32,
						m00 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33,

						m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30,
						m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31,
						m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32,
						m10 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33,

						m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30,
						m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31,
						m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32,
						m20 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33,

						m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30,
						m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31,
						m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32,
						m30 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33);

		return *this = tmp;
	}

	static const tMatrix4x4 identity;
};

template<typename T>
const tMatrix4x4<T> tMatrix4x4<T>::identity = tMatrix4x4<T>(T(1), T(0), T(0), T(0),
															T(0), T(1), T(0), T(0),
															T(0), T(0), T(1), T(0),
															T(0), T(0), T(0), T(1));

template<typename T>
inline tMatrix4x4<T> operator*(T s, const tMatrix4x4<T> &m)
{
	return tMatrix4x4<T>(s * m.m00, s * m.m01, s * m.m02, s * m.m03,
						 s * m.m10, s * m.m11, s * m.m12, s * m.m13,
						 s * m.m20, s * m.m21, s * m.m22, s * m.m23,
						 s * m.m30, s * m.m31, s * m.m32, s * m.m33);
}

template<typename T>
inline tVector3<T> operator*(const tVector3<T> &v, const tMatrix4x4<T> &m)
{
	return tVector3<T>(v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + m.m30,
					   v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + m.m31,
					   v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + m.m32);
}


template<typename T>
inline tVector3<T> &operator*=(tVector3<T> &v, const tMatrix4x4<T> &m)
{
	tVector3<T> tmp(v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + m.m30,
					v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + m.m31,
					v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + m.m32);

	return v = tmp;
}

template<typename T>
inline tVector4<T> operator*(const tVector4<T> &v, const tMatrix4x4<T> &m)
{
	return tVector4<T>(v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + v.w * m.m30,
					   v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + v.w * m.m31,
					   v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + v.w * m.m32,
					   v.x * m.m03 + v.y * m.m13 + v.z * m.m23 + v.w * m.m33);
}

template<typename T>
inline void get_basis(tMatrix3x3<T> &basis, const tMatrix4x4<T> &m)
{
	basis.m00 = m.m00; basis.m01 = m.m01; basis.m02 = m.m02;
	basis.m10 = m.m10; basis.m11 = m.m11; basis.m12 = m.m12;
	basis.m20 = m.m20; basis.m21 = m.m21; basis.m22 = m.m22;
}

template<typename T>
inline tMatrix3x3<T> get_unscaled_basis(const tMatrix4x4<T> &m)
{
	return tMatrix3x3<T>(normalize(m.rows[0].xyz), 
						 normalize(m.rows[1].xyz), 
						 normalize(m.rows[2].xyz));
}

template<typename T>
inline void set_basis(tMatrix4x4<T> &m, const tMatrix3x3<T> &basis)
{
	m.m00 = basis.m00; m.m01 = basis.m01; m.m02 = basis.m02; 
	m.m10 = basis.m10; m.m11 = basis.m11; m.m12 = basis.m12; 
	m.m20 = basis.m20; m.m21 = basis.m21; m.m22 = basis.m22; 
}

template<typename T>
inline void get_origin(tVector3<T> &origin, const tMatrix4x4<T> &m)
{
	origin.x = m.m30; origin.y = m.m31; origin.z = m.m32;
}

template<typename T>
inline void set_origin(tMatrix4x4<T> &m, const tVector3<T> &origin)
{
	m.m30 = origin.x; m.m31 = origin.y; m.m32 = origin.z; 
}

template<typename T>
inline tVector3<T> get_scale(const tMatrix4x4<T> &m)
{
	return tVector3<T>(length(m.rows[0].xyz), length(m.rows[1].xyz), length(m.rows[2].xyz));
}

template<typename T>
inline void set_scale(tMatrix4x4<T> &m, T x, T y, T z)
{
	m.m00 = x;    m.m01 = T(0); m.m02 = T(0); m.m03 = T(0);
	m.m10 = T(0); m.m11 = y;    m.m12 = T(0); m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 = z;    m.m23 = T(0);
	m.m30 = T(0); m.m31 = T(0); m.m32 = T(0); m.m33 = T(1); 
}

template<typename T>
inline void set_scale(tMatrix4x4<T> &m, const tVector3<T> &v)
{
	m.m00 = v.x;  m.m01 = T(0); m.m02 = T(0); m.m03 = T(0);
	m.m10 = T(0); m.m11 = v.y;  m.m12 = T(0); m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 = v.z;  m.m23 = T(0);
	m.m30 = T(0); m.m31 = T(0); m.m32 = T(0); m.m33 = T(1); 
}

template<typename T>
inline void scale(tMatrix4x4<T> &m, const tVector3<T> &v)
{
	m.m00 *= v.x; m.m01 *= v.x; m.m02 *= v.x; 
	m.m10 *= v.y; m.m11 *= v.y; m.m12 *= v.y;
	m.m20 *= v.z; m.m21 *= v.z; m.m22 *= v.z;
}

template<typename T>
inline void set_translation(tMatrix4x4<T> &m, T x, T y, T z)
{
	m.m00 = T(1); m.m01 = T(0); m.m02 = T(0); m.m03 = T(0);
	m.m10 = T(0); m.m11 = T(1); m.m12 = T(0); m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 = T(1); m.m23 = T(0);
	m.m30 = x;    m.m31 = y;    m.m32 = z;    m.m33 = T(1);	
}

template<typename T>
inline void set_translation(tMatrix4x4<T> &m, const tVector3<T> &v)
{
	m.m00 = T(1); m.m01 = T(0); m.m02 = T(0); m.m03 = T(0);
	m.m10 = T(0); m.m11 = T(1); m.m12 = T(0); m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 = T(1); m.m23 = T(0);
	m.m30 = v.x;  m.m31 = v.y;  m.m32 = v.z;  m.m33 = T(1);	
}

template<typename T>
inline void set_rotation_x(tMatrix4x4<T> &m, T a)
{
	T c = cos(a);
	T s = sin(a);

	m.m00 = T(1); m.m01 = T(0); m.m02 =  T(0); m.m03 = T(0);
	m.m10 = T(0); m.m11 = c;    m.m12 = -s;    m.m13 = T(0);
	m.m20 = T(0); m.m21 = s;    m.m22 =  c;    m.m23 = T(0);
	m.m30 = T(0); m.m31 = T(0); m.m32 =  T(0); m.m33 = T(1);
}

template<typename T>
inline void set_rotation_y(tMatrix4x4<T> &m, T a)
{
	T c = cos(a);
	T s = sin(a);

	m.m00 =  c;    m.m01 = T(0); m.m02 = s;    m.m03 = T(0);
	m.m10 =  T(0); m.m11 = T(1); m.m12 = T(0); m.m13 = T(0);
	m.m20 = -s;    m.m21 = T(0); m.m22 = c;    m.m23 = T(0);
	m.m30 =  T(0); m.m31 = T(0); m.m32 = T(0); m.m33 = T(1);
}

template<typename T>
inline void set_rotation_z(tMatrix4x4<T> &m, T a)
{
	T c = cos(a);
	T s = sin(a);

	m.m00 = c;    m.m01 = -s;   m.m02 = T(0); m.m03 = T(0);
	m.m10 = s;    m.m11 = c;    m.m12 = T(0); m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 = T(1); m.m23 = T(0);
	m.m30 = T(0); m.m31 = T(0); m.m32 = T(0); m.m33 = T(1);
}

template<typename T>
inline void set_rotation(tMatrix4x4<T> &m, const tVector3<T> &v, T a)
{
	T c = cos(a);
	T s = sin(a);
	T t = T(1) - c;

	m.m00 = c + v.x * v.x * t;
	m.m11 = c + v.y * v.y * t;
	m.m22 = c + v.z * v.z * t;

	T tmp1 = v.x * v.y * t;
	T tmp2 = v.z * s;

	m.m10 = tmp1 + tmp2;
	m.m01 = tmp1 - tmp2;

	tmp1 = v.x * v.z * t;
	tmp2 = v.y * s;

	m.m20 = tmp1 - tmp2;
	m.m02 = tmp1 + tmp2;   
		
	tmp1 = v.y * v.z * t;
	tmp2 = v.x * s;

	m.m21 = tmp1 + tmp2;
	m.m12 = tmp1 - tmp2;


	m.m03 = T(0); m.m13 = T(0); m.m23 = T(0);
	m.m30 = T(0); m.m31 = T(0); m.m32 = T(0); m.m33 = T(1);
}

template<typename T>
inline void set_reflection(tMatrix4x4<T> &m, const Plane& p);

template<typename T>
inline tMatrix4x4<T> transpose(const tMatrix4x4<T> &m)
{
	return tMatrix4x4<T>(m.m00, m.m10, m.m20, m.m30, 
						 m.m01, m.m11, m.m21, m.m31,
						 m.m02, m.m12, m.m22, m.m32,
						 m.m03, m.m13, m.m23, m.m33);
}

template<typename T>
inline T det(const tMatrix4x4<T> &m)
{
	return m.m00 * m.m11 * m.m22 * m.m33 + m.m00 * m.m12 * m.m23 * m.m31 + m.m00 * m.m13 * m.m21 * m.m32
		 + m.m01 * m.m10 * m.m23 * m.m32 + m.m01 * m.m12 * m.m20 * m.m33 + m.m01 * m.m13 * m.m22 * m.m30
		 + m.m02 * m.m10 * m.m21 * m.m33 + m.m02 * m.m11 * m.m23 * m.m30 + m.m02 * m.m13 * m.m20 * m.m31
		 + m.m03 * m.m10 * m.m22 * m.m31 + m.m03 * m.m11 * m.m21 * m.m32 + m.m03 * m.m12 * m.m21 * m.m30

		 - m.m00 * m.m11 * m.m23 * m.m32 - m.m00 * m.m12 * m.m21 * m.m33 - m.m00 * m.m13 * m.m22 * m.m31
		 - m.m01 * m.m10 * m.m22 * m.m33 - m.m01 * m.m12 * m.m23 * m.m30 - m.m01 * m.m13 * m.m20 * m.m32
		 - m.m02 * m.m10 * m.m23 * m.m31 - m.m02 * m.m11 * m.m20 * m.m33 - m.m02 * m.m13 * m.m21 * m.m30
		 - m.m03 * m.m10 * m.m21 * m.m32 - m.m03 * m.m11 * m.m22 * m.m30 - m.m03 * m.m12 * m.m20 * m.m31;
}

template<typename T>
inline tMatrix4x4<T> invert(const tMatrix4x4<T> &m)
{
	return tMatrix4x4<T>(m.m11*m.m22*m.m33 + m.m12*m.m23*m.m31 + m.m13*m.m21*m.m32 - m.m11*m.m23*m.m32 - m.m12*m.m21*m.m33 - m.m13*m.m22*m.m31,
						 m.m01*m.m23*m.m32 + m.m02*m.m21*m.m33 + m.m03*m.m22*m.m31 - m.m01*m.m22*m.m33 - m.m02*m.m23*m.m31 - m.m03*m.m21*m.m32,
					     m.m01*m.m12*m.m33 + m.m02*m.m13*m.m31 + m.m03*m.m11*m.m32 - m.m01*m.m13*m.m32 - m.m02*m.m11*m.m33 - m.m03*m.m12*m.m31,
						 m.m01*m.m13*m.m22 + m.m02*m.m11*m.m23 + m.m03*m.m12*m.m21 - m.m01*m.m12*m.m23 - m.m02*m.m13*m.m21 - m.m03*m.m11*m.m22,

						 m.m10*m.m23*m.m32 + m.m12*m.m20*m.m33 + m.m13*m.m22*m.m30 - m.m10*m.m22*m.m33 - m.m12*m.m23*m.m30 - m.m13*m.m20*m.m32,
						 m.m00*m.m22*m.m33 + m.m02*m.m23*m.m30 + m.m03*m.m20*m.m32 - m.m00*m.m23*m.m32 - m.m02*m.m20*m.m33 - m.m03*m.m22*m.m30,
						 m.m00*m.m13*m.m32 + m.m02*m.m10*m.m33 + m.m03*m.m12*m.m30 - m.m00*m.m12*m.m33 - m.m02*m.m13*m.m30 - m.m03*m.m10*m.m32,
						 m.m00*m.m12*m.m23 + m.m02*m.m13*m.m20 + m.m03*m.m10*m.m22 - m.m00*m.m13*m.m22 - m.m02*m.m10*m.m23 - m.m03*m.m12*m.m20,

						 m.m10*m.m21*m.m33 + m.m11*m.m23*m.m30 + m.m13*m.m20*m.m31 - m.m10*m.m23*m.m31 - m.m11*m.m20*m.m33 - m.m13*m.m21*m.m30,
						 m.m00*m.m23*m.m31 + m.m01*m.m20*m.m33 + m.m03*m.m21*m.m30 - m.m00*m.m21*m.m33 - m.m01*m.m23*m.m30 - m.m03*m.m20*m.m31,
						 m.m00*m.m11*m.m33 + m.m01*m.m13*m.m30 + m.m03*m.m10*m.m31 - m.m00*m.m13*m.m31 - m.m01*m.m10*m.m33 - m.m03*m.m11*m.m30,
						 m.m00*m.m13*m.m21 + m.m01*m.m10*m.m23 + m.m03*m.m11*m.m20 - m.m00*m.m11*m.m23 - m.m01*m.m13*m.m20 - m.m03*m.m10*m.m21,

						 m.m10*m.m22*m.m31 + m.m11*m.m20*m.m32 + m.m12*m.m21*m.m30 - m.m10*m.m21*m.m32 - m.m11*m.m22*m.m30 - m.m12*m.m20*m.m31,
						 m.m00*m.m21*m.m32 + m.m01*m.m22*m.m30 + m.m02*m.m20*m.m31 - m.m00*m.m22*m.m31 - m.m01*m.m20*m.m32 - m.m02*m.m21*m.m30,
						 m.m00*m.m12*m.m31 + m.m01*m.m10*m.m32 + m.m02*m.m11*m.m30 - m.m00*m.m11*m.m32 - m.m01*m.m12*m.m30 - m.m02*m.m10*m.m31,
						 m.m00*m.m11*m.m22 + m.m01*m.m12*m.m20 + m.m02*m.m10*m.m21 - m.m00*m.m12*m.m21 - m.m01*m.m10*m.m22 - m.m02*m.m11*m.m20) / det(m);
}

template<typename T>
inline void set_look_at(tMatrix4x4<T> &m, const tVector3<T> &eye, const tVector3<T> &at, const tVector3<T> &up)
{
	/*
	zaxis = normal(At - Eye)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
   	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye) 1
	*/
	tVector3<T> z = normalize(at - eye);
	tVector3<T> x = normalize(cross(up, z));
	tVector3<T> y = cross(z, x);

	m.m00 =  x.x;		m.m01 = y.x;		m.m02 = z.x;          m.m03 = T(0);
	m.m10 =  x.y;         	m.m11 = y.y;          	m.m12 = z.y;	      m.m13 = T(0);
	m.m20 =  x.z;         	m.m21 = y.z;          	m.m22 = z.z;          m.m23 = T(0);
	m.m30 = -dot(x, eye); 	m.m31 = -dot(y, eye); 	m.m32 = -dot(z, eye); m.m33 = T(1);
}

template<typename T>
inline void set_look_at_negative_x(tMatrix4x4<T> &m, const tVector3<T> &eye, const tVector3<T> &at, const tVector3<T> &up)
{
	tVector3<T> z = normalize(at - eye);
	tVector3<T> x = normalize(cross(up, z));
	tVector3<T> y = cross(z, x);

	m.m00 = -x.x;		m.m01 = y.x;		m.m02 = z.x;          m.m03 = T(0);
	m.m10 = -x.y;         	m.m11 = y.y;          	m.m12 = z.y;	      m.m13 = T(0);
	m.m20 = -x.z;         	m.m21 = y.z;          	m.m22 = z.z;          m.m23 = T(0);
	m.m30 = -dot(x, eye); 	m.m31 = -dot(y, eye); 	m.m32 = -dot(z, eye); m.m33 = T(1);
}

template<typename T>
inline void set_look_at_negative_y(tMatrix4x4<T> &m, const tVector3<T> &eye, const tVector3<T> &at, const tVector3<T> &up)
{
	tVector3<T> z = normalize(at - eye);
	tVector3<T> x = normalize(cross(up, z));
	tVector3<T> y = cross(z, x);

	m.m00 =  x.x;		    m.m01 = -y.x;		m.m02 = z.x;          m.m03 = T(0);
	m.m10 =  x.y;         	m.m11 = -y.y;          	m.m12 = z.y;	      m.m13 = T(0);
	m.m20 =  x.z;         	m.m21 = -y.z;          	m.m22 = z.z;          m.m23 = T(0);
	m.m30 = -dot(x, eye); 	m.m31 = -dot(y, eye); 	m.m32 = -dot(z, eye); m.m33 = T(1);
}

template<typename T>
inline void set_look_at_RH(tMatrix4x4<T> &m, const tVector3<T> &eye, const tVector3<T> &at, const tVector3<T> &up)
{
	/*
	zaxis = normal(Eye - At)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
   -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
	*/
	tVector3<T> z = normalize(eye - at);
	tVector3<T> x = normalize(cross(up, z));
	tVector3<T> y = cross(z, x);

	m.m00 = x.x;		 m.m01 = y.x;		  m.m02 = z.x; 		   m.m03 = T(0);
	m.m10 = x.y;		 m.m11 = y.y;		  m.m12 = z.y;		   m.m13 = T(0);
	m.m20 = x.z;		 m.m21 = y.z;		  m.m22 = z.z; 		   m.m23 = T(0);
	m.m30 = dot(x, eye); m.m31 = dot(y, eye); m.m32 = dot(z, eye); m.m33 = T(1);
}

template<typename T>
inline void set_perspective(tMatrix4x4<T>& m, T fov, T ratio, T z_near, T z_far, bool upside_down = false)
{
#ifdef CLIP_NEAR_Z_MINUS_ONE

	T f = math::cot(fov * T(0.5));
	T f_n = T(1) / (z_far - z_near);

	m.m00 = f / ratio; m.m01 = T(0); m.m02 =  T(0); 				         m.m03 = T(0);
	m.m10 = T(0);	   m.m11 = f;  	 m.m12 =  T(0);				             m.m13 = T(0);
	m.m20 = T(0);	   m.m21 = T(0); m.m22 =  (z_far + z_near) * f_n; 		 m.m23 = T(1);
	m.m30 = T(0);	   m.m31 = T(0); m.m32 = -(T(2) * z_far * z_near) * f_n; m.m33 = T(0);

//	m.m00 = f / ratio; m.m01 = T(0); m.m02 =  T(0); 				         m.m03 = T(0);
//	m.m10 = T(0);	   m.m11 = f;  	 m.m12 =  T(0);				             m.m13 = T(0);
//	m.m20 = T(0);	   m.m21 = T(0); m.m22 =  (z_far + z_near) / (z_near - z_far); 		 m.m23 = -T(1);
//	m.m30 = T(0);	   m.m31 = T(0); m.m32 = (2) / (z_near * z_far); m.m33 = T(1);

#else

	/*  
	xScale     0          0               0
	0        yScale       0               0
	0          0       zf/(zf-zn)         1
	0          0       -zn*zf/(zf-zn)     0
	where:
	yScale = cot(fovY/2)
	xScale = aspect ratio * yScale
	*/

	T t = fov * T(0.5);
	T y = cos(t) / sin(t);
	T x = y / ratio;

	m.m00 = x;    m.m01 = T(0); m.m02 =  T(0);                  m.m03 = T(0);
	m.m10 = T(0); m.m11 = y;    m.m12 =  T(0);                  m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 =  z_far / (z_far - z_near); m.m23 = T(1);
	m.m30 = T(0); m.m31 = T(0); m.m32 = -z_near * m.m22;         m.m33 = T(0);

#endif

	if (upside_down)
	{
		m.m11 *= -T(1);
	}
}

template<typename T>
inline void set_perspective_linear(tMatrix4x4<T>& m, T fov, T aspect, T znear, T zfar)
{
	/*  
	xScale     0          0               0
	0        yScale       0               0
	0          0       zf/(zf-zn)         1
	0          0       -zn*zf/(zf-zn)     0
	where:
	yScale = cot(fovY/2)
	xScale = aspect ratio * yScale
	*/

	T t = fov * T(0.5);
	T y = cos(t) / sin(t);
	T x = y / aspect;

	m.m00 = x;    m.m01 = T(0); m.m02 =  T(0);                  m.m03 = T(0);
	m.m10 = T(0); m.m11 = y;    m.m12 =  T(0);                  m.m13 = T(0);
	m.m20 = T(0); m.m21 = T(0); m.m22 =  zfar / (zfar - znear); m.m23 = T(1);
	m.m30 = T(0); m.m31 = T(0); m.m32 = -znear * m.m22;         m.m33 = T(0);

	m.m22 /= zfar;
	m.m32 /= zfar;
}

template<typename T>
inline void set_ortho(tMatrix4x4<T>& m, T width, T height, T z_near, T z_far)
{

#ifdef CLIP_NEAR_Z_MINUS_ONE

	T f_n = T(1) / (z_far - z_near);

	m.m00 = T(2) / width; m.m01 = T(0);          m.m02 =  T(0);                   m.m03 = T(0);
	m.m10 = T(0);         m.m11 = T(2) / height; m.m12 =  T(0);                   m.m13 = T(0);
	m.m20 = T(0);         m.m21 = T(0);          m.m22 =  T(2) * f_n;			  m.m23 = T(0);
	m.m30 = T(0);         m.m31 = T(0);          m.m32 = -(z_far + z_near) * f_n; m.m33 = T(1);

#else

	/*
	2/w  0    0           0
	0    2/h  0           0
	0    0    1/(zf-zn)   0
	0    0    zn/(zn-zf)  1
	*/

	m.m00 = T(2) / width; m.m01 = T(0);          m.m02 = T(0);                      m.m03 = T(0);
	m.m10 = T(0);         m.m11 = T(2) / height; m.m12 = T(0);                      m.m13 = T(0);
	m.m20 = T(0);         m.m21 = T(0);          m.m22 = T(1) / (z_far - z_near);   m.m23 = T(0);
	m.m30 = T(0);         m.m31 = T(0);          m.m32 = z_near / (z_near - z_far); m.m33 = T(1);

#endif
}

template<typename T>
inline void set_clip(tMatrix4x4<T> &m, T cx, T cy, T cw, T ch, T zmin, T zmax)
{
	/*
	2/cw		0			0				 0
	0			2/ch		0				 0
	0			0			1/(zmax-zmin)	 0
	-1-2(cx/cw)	1-2(cy/ch) -zmin/(zmax-zmin)	 1
	*/

	const T zdif = zmax - zmin;

	m.m00 = T(2) / cw;                m.m01 = T(0);                     m.m02 = T(0);         m.m03 = T(0);
	m.m10 = T(0);                     m.m11 = T(2) / ch;                m.m12 = T(0);         m.m13 = T(0);
	m.m20 = T(0);                     m.m21 = T(0);                     m.m22 = T(1) / zdif;  m.m23 = T(0);
	m.m30 = -T(1) - T(2) * (cx / cw); m.m31 = -T(1) - T(2) * (cy / ch); m.m32 = -zmin / zdif; m.m33 = T(1);
}

template<typename T>
inline void set_viewport_scale(tMatrix4x4<T> &m, T x, T y, T width, T height)
{
	/*
	width	0			0	0
	0		-height		0	0
	0		0			1	0
	x		height+y	0	1
	*/

	m.m00 = width; m.m01 = T(0);       m.m02 = T(0); m.m03 = T(0);
	m.m10 = T(0);  m.m11 = -height;    m.m12 = T(0); m.m13 = T(0);
	m.m20 = T(0);  m.m21 = T(0);       m.m22 = T(1); m.m23 = T(0);
	m.m30 = x;     m.m31 = height + y; m.m32 = T(0); m.m33 = T(1);
}

template<typename T>
inline tVector2<T> project_to_screen(const tVector3<T> &v, const tMatrix4x4<T> &worldViewProj, float halfW, float halfH, float x, float y)
{
	tVector4<T> t = tVector4<T>(v.x, v.y, v.z, T(1)) * worldViewProj;
	tVector2<T> o = tVector2<T>(t.x, t.y) / t.w;

	o.x = (o.x + T(1)) * halfW + x;
	o.y = (T(1) - o.y) * halfH + y;

	return o;
}

template<typename T>
inline tVector3<T> project_to_screen(const tVector3<T> &v, const tMatrix4x4<T> &worldViewProj, float halfW, float halfH, float x, float y, float zmin, float zmax)
{
	tVector4<T> t = tVector4<T>(v.x, v.y, v.z, T(1)) * worldViewProj;
	tVector3<T> o = tVector3<T>(t.x, t.y, t.z) / t.w;

	o.x = (o.x + T(1)) * halfW + x;
	o.y = (T(1) - o.y) * halfH + y;
	o.z = zmin + (zmax - zmin) * o.z;

	return o;
}