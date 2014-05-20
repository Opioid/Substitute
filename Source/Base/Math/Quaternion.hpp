#pragma once

template<typename T> struct tVector3;
template<typename T> struct tMatrix3x3;

template<typename T>
struct tQuaternion
{
	T x, y, z, w;


	tQuaternion()
	{}


	tQuaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w)
	{}


	explicit tQuaternion(const tMatrix3x3<T> &m)
	{
		T trace = m.m00 + m.m11 + m.m22;
		T temp[4];

		if (trace > T(0))
		{
			T s = sqrt(trace + T(1));
			temp[3] = s * T(0.5);
			s = T(0.5) / s;

			temp[0] = (m.m21 - m.m12) * s;
			temp[1] = (m.m02 - m.m20) * s;
			temp[2] = (m.m10 - m.m01) * s;
		}
		else
		{
			int i = m.m00 < m.m11 ? (m.m11 < m.m22 ? 2 : 1) :(m.m00 < m.m22 ? 2 : 0); 
			int j = (i + 1) % 3;  
			int k = (i + 2) % 3;

			T s = sqrt(m.m[i * 3 + i] - m.m[j * 3 + j] - m.m[k * 3 + k] + T(1));
			temp[i] = s * T(0.5);
			s = T(0.5) / s;

			temp[3] = (m.m[k * 3 + j] - m.m[j * 3 + k]) * s;
			temp[j] = (m.m[j * 3 + i] + m.m[i * 3 + j]) * s;
			temp[k] = (m.m[k * 3 + i] + m.m[i * 3 + k]) * s;
		}

		x = temp[0];
		y = temp[1];
		z = temp[2];
		w = temp[3];
	}


	tQuaternion operator*(const tQuaternion &q) const
	{
		return tQuaternion(w * q.x + x * q.w + y * q.z - z * q.y,
						   w * q.y + y * q.w + z * q.x - x * q.z,
						   w * q.z + z * q.w + x * q.y - y * q.x,
						   w * q.w - x * q.x - y * q.y - z * q.z); 
	}


	tQuaternion &operator*=(const tQuaternion &q)
	{
		tQuaternion tmp(w * q.x + x * q.w + y * q.z - z * q.y,
						w * q.y + y * q.w + z * q.x - x * q.z,
						w * q.z + z * q.w + x * q.y - y * q.x,
						w * q.w - x * q.x - y * q.y - z * q.z); 

		return *this = tmp;
	}


	static const tQuaternion identity;
};


template<typename T>
const tQuaternion<T> tQuaternion<T>::identity(T(0), T(0), T(0), T(1));


template<typename T>
inline T dot(const tQuaternion<T> &a, const tQuaternion<T> &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}


template<typename T>
inline T length(const tQuaternion<T> &q)
{
	return sqrt(dot(q, q));
}


template<typename T>
inline T angle(const tQuaternion<T> &a, const tQuaternion<T> &b)
{
	T s = sqrt(dot(a, a) * dot(b, b));
	return acos(dot(a, b) / s);
}


template<typename T>
inline void set_rotation_x(tQuaternion<T> &q, T a)
{
	q.x = sin(a * T(0.5));
	q.y = T(0);
	q.z = T(0);
	q.w = cos(a * T(0.5));
}

template<typename T>
inline void set_rotation_y(tQuaternion<T> &q, T a)
{
	q.x = T(0);
	q.y = sin(a * T(0.5));
	q.z = T(0);
	q.w = cos(a * T(0.5));
}

template<typename T>
inline void set_rotation_z(tQuaternion<T> &q, T a)
{
	q.x = T(0);
	q.y = T(0);
	q.z = sin(a * T(0.5));
	q.w = cos(a * T(0.5));
}


template<typename T>
inline void set_rotation(tQuaternion<T> &q, const tVector3<T> &v, T a)
{
	T d = length(v);

	T s = sin(a * T(0.5)) / d;

	q.x = v.x * s;
	q.y = v.y * s;
	q.z = v.z * s;
	q.w = cos(a * T(0.5));
}


template<typename T>
inline void set_rotation(tQuaternion<T> &q, T yaw, T pitch, T roll)
{
	/*	btScalar halfYaw = btScalar(yaw) * btScalar(0.5);  
		btScalar halfPitch = btScalar(pitch) * btScalar(0.5);  
		btScalar halfRoll = btScalar(roll) * btScalar(0.5);  
		btScalar cosYaw = btCos(halfYaw);
		btScalar sinYaw = btSin(halfYaw);
		btScalar cosPitch = btCos(halfPitch);
		btScalar sinPitch = btSin(halfPitch);
		btScalar cosRoll = btCos(halfRoll);
		btScalar sinRoll = btSin(halfRoll);
		setValue(cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
				 cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
				 sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
				 cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw);
			*/

	T halfYaw   = yaw   * T(0.5);
	T halfPitch = pitch * T(0.5);
	T halfRoll  = roll  * T(0.5);

	T cosYaw = cos(halfYaw);
	T sinYaw = sin(halfYaw);
	T cosPitch = cos(halfPitch);
	T sinPitch = sin(halfPitch);
	T cosRoll = cos(halfRoll);
	T sinRoll = sin(halfRoll);


	q.x = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
	q.y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
	q.z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
	q.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

/*
	q.x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
	q.y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
	q.z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
	q.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
	*/
}


template<typename T>
inline tQuaternion<T> slerp(const tQuaternion<T> &a, const tQuaternion<T> &b, T t)
{
/*	T theta = angle(a, b);

	if (theta != T(0))
	{
		T d = T(1) / sin(theta);
		T s0 = sin((T(1) - t) * theta);
		T s1 = sin(t * theta);

		return tQuaternion<T>((a.x * s0 + b.x * s1) * d,
							   (a.y * s0 + b.y * s1) * d,
							   (a.z * s0 + b.z * s1) * d,
							   (a.w * s0 + b.w * s1) * d);
	}
	else return a;
	*/


	

	// calc cosine theta
	float cosom = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

	// adjust signs (if necessary)
	tQuaternion<T> end = b;

	if (cosom < T(0))
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	} 

	// Calculate coefficients
	float sclp, sclq;

	if (T(1) - cosom > T(0.0001)) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		float omega = acos(cosom); // extract theta from dot product's cos theta
		float sinom = sin(omega);
		sclp  = sin((T(1) - t) * omega) / sinom;
		sclq  = sin(t * omega) / sinom;
	} 
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = T(1) - t;
		sclq = t;
	}

	return tQuaternion<T>(sclp * a.x + sclq * end.x,
						   sclp * a.y + sclq * end.y,
						   sclp * a.z + sclq * end.z,
						   sclp * a.w + sclq * end.w);
}


typedef tQuaternion<float> Quaternion;
