#pragma once

#include "Vector3.hpp"
#include "Quaternion.hpp"

template<typename T> struct tMatrix4x4;

template<typename T> 
struct tMatrix3x3
{
	union
	{
		struct
		{
			T m00, m01, m02,
			  m10, m11, m12, 
			  m20, m21, m22;
		};

		struct 
		{
			T m[9]; 
		};

		struct 
		{
			tVector3<T> rows[3];
		};

		struct 
		{
			tVector3<T> x;
			tVector3<T> y;
			tVector3<T> z;
		};
	};

	tMatrix3x3()
	{}

	tMatrix3x3(T m00, T m01, T m02, 
			   T m10, T m11, T m12, 
			   T m20, T m21, T m22) :
		m00(m00), m01(m01), m02(m02),
		m10(m10), m11(m11), m12(m12),
		m20(m20), m21(m21), m22(m22)
	{}
		
	tMatrix3x3(const tVector3<T>& x, const tVector3<T>& y, const tVector3<T>& z) : x(x), y(y), z(z)
	{}
	
	explicit tMatrix3x3(const tMatrix4x4<T>& m) :
		m00(m.m00), m01(m.m01), m02(m.m02),
		m10(m.m10), m11(m.m11), m12(m.m12),
		m20(m.m20), m21(m.m21), m22(m.m22)
	{}

	explicit tMatrix3x3(const tQuaternion<T>& q)
	{
		T d = dot(q, q);

		T s = T(2) / d;

		T xs = q.x * s,  ys = q.y * s,  zs = q.z * s;
		T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

		m00 = T(1) - (yy + zz); m01 = xy - wz;          m02 = xz + wy;
		m10 = xy + wz;          m11 = T(1) - (xx + zz); m12 = yz - wx;
		m20 = xz - wy;          m21 = yz + wx,          m22 = T(1) - (xx + yy);
	}

	tMatrix3x3 operator*(const tMatrix3x3& m) const
	{
		return tMatrix3x3(m00 * m.m00 + m01 * m.m10 + m02 * m.m20,
						  m00 * m.m01 + m01 * m.m11 + m02 * m.m21,
						  m00 * m.m02 + m01 * m.m12 + m02 * m.m22,

						  m10 * m.m00 + m11 * m.m10 + m12 * m.m20,
						  m10 * m.m01 + m11 * m.m11 + m12 * m.m21,
						  m10 * m.m02 + m11 * m.m12 + m12 * m.m22,

						  m20 * m.m00 + m21 * m.m10 + m22 * m.m20,
						  m20 * m.m01 + m21 * m.m11 + m22 * m.m21,
						  m20 * m.m02 + m21 * m.m12 + m22 * m.m22);
	}

	tMatrix3x3 operator/(T s) const
	{
		T is = T(1) / s;
		return tMatrix3x3<T>(is * m00, is * m01, is * m02,
							 is * m10, is * m11, is * m12,
							 is * m20, is * m21, is * m22);
	}

	tMatrix3x3& operator*=(const tMatrix3x3& m)
	{
		tMatrix3x3 tmp(m00 * m.m00 + m01 * m.m10 + m02 * m.m20,
					   m00 * m.m01 + m01 * m.m11 + m02 * m.m21,
					   m00 * m.m02 + m01 * m.m12 + m02 * m.m22,

					   m10 * m.m00 + m11 * m.m10 + m12 * m.m20,
					   m10 * m.m01 + m11 * m.m11 + m12 * m.m21,
					   m10 * m.m02 + m11 * m.m12 + m12 * m.m22,

					   m20 * m.m00 + m21 * m.m10 + m22 * m.m20,
					   m20 * m.m01 + m21 * m.m11 + m22 * m.m21,
					   m20 * m.m02 + m21 * m.m12 + m22 * m.m22);

		return *this = tmp;
	}

	static const tMatrix3x3 identity;
};

template<typename T>
const tMatrix3x3<T> tMatrix3x3<T>::identity = tMatrix3x3<T>(T(1), T(0), T(0),
															T(0), T(1), T(0),
															T(0), T(0), T(1));

template<typename T>
inline tVector3<T> operator*(const tVector3<T>& v, const tMatrix3x3<T>& m)
{
	return tVector3<T>(v.x * m.m00 + v.y * m.m10 + v.z * m.m20,
					   v.x * m.m01 + v.y * m.m11 + v.z * m.m21,
					   v.x * m.m02 + v.y * m.m12 + v.z * m.m22);
}

template<typename T>
inline tVector3<T>& operator*=(tVector3<T>& v, const tMatrix3x3<T>& m)
{
	tVector3<T> tmp(v.x * m.m00 + v.y * m.m10 + v.z * m.m20,
					v.x * m.m01 + v.y * m.m11 + v.z * m.m21,
					v.x * m.m02 + v.y * m.m12 + v.z * m.m22);

	return v = tmp;
}

template<typename T>
inline tVector3<T> operator*(const tMatrix3x3<T>& m, const tVector3<T>& v)
{
	return tVector3<T>(v.x * m.m00 + v.y * m.m01 + v.z * m.m02,
					   v.x * m.m10 + v.y * m.m11 + v.z * m.m12,
					   v.x * m.m20 + v.y * m.m21 + v.z * m.m22);
}

template<typename T>
inline tMatrix3x3<T> normalize(const tMatrix3x3<T>& m)
{
	T s0 = T(1) / length(tVector3<T>(m.m00, m.m01, m.m02));
	T s1 = T(1) / length(tVector3<T>(m.m10, m.m11, m.m12));
	T s2 = T(1) / length(tVector3<T>(m.m20, m.m21, m.m22));

	return tMatrix3x3<T>(s0 * m.m00, s0 * m.m01, s0 * m.m02,
						 s1 * m.m10, s1 * m.m11, s1 * m.m12,
						 s2 * m.m20, s2 * m.m21, s2 * m.m22);
}

template<typename T>
inline T det(const tMatrix3x3<T>& m)
{
	return m.m00 * m.m11 * m.m22  
		 + m.m10 * m.m21 * m.m02 
		 + m.m20 * m.m01 * m.m12 
		 - m.m00 * m.m21 * m.m12  
		 - m.m20 * m.m11 * m.m02 
		 - m.m10 * m.m01 * m.m22;
}

template<typename T>
inline tMatrix3x3<T> invert(const tMatrix3x3<T>& m)
{
	return tMatrix3x3<T>(m.m11 * m.m22 - m.m12 * m.m21, m.m02 * m.m21 - m.m01 * m.m22, m.m01 * m.m12 - m.m02 * m.m11,
						 m.m12 * m.m20 - m.m10 * m.m22, m.m00 * m.m22 - m.m02 * m.m20, m.m02 * m.m10 - m.m00 * m.m12,
						 m.m10 * m.m21 - m.m11 * m.m20, m.m01 * m.m20 - m.m00 * m.m21, m.m00 * m.m11 - m.m01 * m.m10) / det(m);
}

template<typename T>
inline void set_look_at(tMatrix3x3<T>& m, const tVector3<T> &dir, const tVector3<T> &up)
{
	tVector3<T> z = normalize(dir);
	tVector3<T> x = normalize(cross(up, z));
	tVector3<T> y = cross(z, x);

	/*
	m.m00 =  x.x;		  m.m01 = y.x;		    m.m02 = z.x;          
	m.m10 =  x.y;         m.m11 = y.y;          m.m12 = z.y;	      
	m.m20 =  x.z;         m.m21 = y.z;          m.m22 = z.z;     
	*/
	//Confusion!

	m.m00 =  x.x;		  m.m01 = x.y;		    m.m02 = x.z;       
	m.m10 =  y.x;         m.m11 = y.y;          m.m12 = y.z;	    
	m.m20 =  z.x;         m.m21 = z.y;          m.m22 = z.z;         
}

template<typename T>
inline void set_basis(tMatrix3x3<T>& m, const tVector3<T>& v)
{
	m.rows[2] = v;

	if (v.x < T(0.6) && v.x > -T(0.6)) 
	{
		m.rows[1] = tVector3<T>(T(1), T(0), T(0));
	}
	else if (v.y < T(0.6) && v.y > T(0.6)) 
	{
		m.rows[1] = tVector3<T>(T(0), T(1), T(0));
	}
	else 
	{
		m.rows[1] = tVector3<T>(T(0), T(0), T(1));
	}
	
	m.rows[0] = normalize(cross(v, m.rows[1]));
	m.rows[1] = cross(m.rows[0], m.rows[2]);
}

template<typename T>
inline void set_scale(tMatrix3x3<T>& m, T x, T y, T z)
{
	m.m00 = x;    m.m01 = T(0); m.m02 = T(0); 
	m.m10 = T(0); m.m11 = y;    m.m12 = T(0); 
	m.m20 = T(0); m.m21 = T(0); m.m22 = z;    
}

template<typename T>
inline void set_scale(tMatrix3x3<T>& m, const tVector3<T>& v)
{
	m.m00 = v.x;  m.m01 = T(0); m.m02 = T(0); 
	m.m10 = T(0); m.m11 = v.y;  m.m12 = T(0); 
	m.m20 = T(0); m.m21 = T(0); m.m22 = v.z;  
}

template<typename T>
inline void scale(tMatrix3x3<T>& m, const tVector3<T>& v)
{
	m.m00 *= v.x; m.m01 *= v.x; m.m02 *= v.x; 
	m.m10 *= v.y; m.m11 *= v.y; m.m12 *= v.y;
	m.m20 *= v.z; m.m21 *= v.z; m.m22 *= v.z;
}

template<typename T>
inline void set_rotation_x(tMatrix3x3<T>& m, T a)
{
	T c = cos(a);
	T s = sin(a);

	m.m00 = T(1); m.m01 = T(0); m.m02 = T(0);   
	m.m10 = T(0); m.m11 = c;   m.m12 = -s;
	m.m20 = T(0); m.m21 = s;   m.m22 =  c;
}

template<typename T>
inline void set_rotation_y(tMatrix3x3<T>& m, T a)
{
	T c = cos(a);
	T s = sin(a);

	m.m00 =  c;    m.m01 = T(0); m.m02 = s;
	m.m10 =  T(0); m.m11 = T(1); m.m12 = T(0);
	m.m20 = -s;    m.m21 = T(0); m.m22 = c;
}

template<typename T>
inline void set_rotation_z(tMatrix3x3<T>& m, T a)
{
	T c = cos(a);
	T s = sin(a);

	m.m00 = c;    m.m01 = -s;    m.m02 = T(0);
	m.m10 = s;    m.m11 =  c;    m.m12 = T(0);
	m.m20 = T(0); m.m21 =  T(0); m.m22 = T(1);
}

template<typename T>
inline void set_rotation(tMatrix3x3<T>& m, const tVector3<T>& v, T a)
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
}


template<typename T>
inline tMatrix3x3<T> transposed(const tMatrix3x3<T>& m)
{
	return tMatrix3x3<T>(m.m00, m.m10, m.m20,
						 m.m01, m.m11, m.m21,
						 m.m02, m.m12, m.m22);
}
