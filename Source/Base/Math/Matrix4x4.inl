#include "Matrix4x4.hpp"
#include "Plane.hpp"


template<typename T>
inline void set_reflection(tMatrix4x4<T> &m, const Plane& p)
{
	m.m00 = -T(2) * p.a * p.a + T(1); m.m01 = -T(2) * p.b * p.a;        m.m02 = -T(2) * p.c * p.a;        m.m03 = T(0);
	m.m10 = -T(2) * p.a * p.b;        m.m11 = -T(2) * p.b * p.b + T(1); m.m12 = -T(2) * p.c * p.b;        m.m13 = T(0);
	m.m20 = -T(2) * p.a * p.c;        m.m21 = -T(2) * p.b * p.c;        m.m22 = -T(2) * p.c * p.c + T(1); m.m23 = T(0);
	m.m30 = -T(2) * p.a * p.d,        m.m31 = -T(2) * p.b * p.d,        m.m32 = -T(2) * p.c * p.d;        m.m33 = T(1);
}