#pragma once

template<typename T> struct Vector2;
template<typename T> struct Vector3;
template<typename T> struct Vector4;
template<typename T> struct Matrix3x3;

struct Plane;

template<typename T>
struct Matrix4x4
{
	union
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
			Vector4<T> rows[4];
		};
	};

	Matrix4x4();

	Matrix4x4(T m00, T m01, T m02, T m03,
			  T m10, T m11, T m12, T m13,
			  T m20, T m21, T m22, T m23,
			  T m30, T m31, T m32, T m33);
		
	Matrix4x4(const T m[16]);

	explicit Matrix4x4(const Matrix3x3<T>& m);

	Matrix4x4 operator*(const Matrix4x4& m) const;

	Matrix4x4 operator/(T s) const;

	Matrix4x4& operator*=(const Matrix4x4& m);

	static const Matrix4x4 identity;
};

template<typename T>
Matrix4x4<T> operator*(T s, const Matrix4x4<T>& m);

template<typename T>
Vector3<T> operator*(const Vector3<T>& v, const Matrix4x4<T>& m);

template<typename T>
Vector3<T> &operator*=(Vector3<T>& v, const Matrix4x4<T>& m);

template<typename T>
Vector4<T> operator*(const Vector4<T>& v, const Matrix4x4<T>& m);

template<typename T>
void get_basis(Matrix3x3<T>& basis, const Matrix4x4<T>& m);

template<typename T>
Matrix3x3<T> extract_unscaled_basis(const Matrix4x4<T>& m);

template<typename T>
void set_basis(Matrix4x4<T>& m, const Matrix3x3<T>& basis);

template<typename T>
void get_origin(Vector3<T> &origin, const Matrix4x4<T>& m);

template<typename T>
void set_origin(Matrix4x4<T>& m, const Vector3<T>& origin);

template<typename T>
Vector3<T> get_scale(const Matrix4x4<T>& m);

template<typename T>
void set_scale(Matrix4x4<T>& m, T x, T y, T z);

template<typename T>
void set_scale(Matrix4x4<T>& m, const Vector3<T>& v);

template<typename T>
void scale(Matrix4x4<T>& m, const Vector3<T>& v);

template<typename T>
void set_translation(Matrix4x4<T>& m, T x, T y, T z);

template<typename T>
void set_translation(Matrix4x4<T>& m, const Vector3<T>& v);

template<typename T>
void set_rotation_x(Matrix4x4<T>& m, T a);

template<typename T>
void set_rotation_y(Matrix4x4<T>& m, T a);

template<typename T>
void set_rotation_z(Matrix4x4<T>& m, T a);

template<typename T>
void set_rotation(Matrix4x4<T>& m, const Vector3<T>& v, T a);

template<typename T>
void set_reflection(Matrix4x4<T>& m, const Plane& p);

template<typename T>
Matrix4x4<T> transpose(const Matrix4x4<T>& m);

template<typename T>
T det(const Matrix4x4<T>& m);

template<typename T>
Matrix4x4<T> invert(const Matrix4x4<T>& m);

template<typename T>
void set_look_at(Matrix4x4<T>& m, const Vector3<T>& eye, const Vector3<T>& at, const Vector3<T>& up);

template<typename T>
void set_look_at_negative_x(Matrix4x4<T>& m, const Vector3<T>& eye, const Vector3<T>& at, const Vector3<T>& );

template<typename T>
void set_look_at_negative_y(Matrix4x4<T>& m, const Vector3<T>& eye, const Vector3<T>& at, const Vector3<T>& up);

template<typename T>
void set_look_at_RH(Matrix4x4<T>& m, const Vector3<T>& eye, const Vector3<T>& at, const Vector3<T>& up);

template<typename T>
void set_perspective(Matrix4x4<T>& m, T fov, T ratio, T z_near, T z_far, bool upside_down = false);

template<typename T>
void set_perspective_linear(Matrix4x4<T>& m, T fov, T aspect, T znear, T zfar);

template<typename T>
void set_ortho(Matrix4x4<T>& m, T width, T height, T z_near, T z_far);

template<typename T>
void set_clip(Matrix4x4<T>& m, T cx, T cy, T cw, T ch, T zmin, T zmax);

template<typename T>
void set_viewport_scale(Matrix4x4<T>& m, T x, T y, T width, T height);

template<typename T>
Vector2<T> project_to_screen(const Vector3<T>& v, const Matrix4x4<T>& worldViewProj, float halfW, float halfH, float x, float y);

template<typename T>
Vector3<T> project_to_screen(const Vector3<T>& v, const Matrix4x4<T>& worldViewProj, float halfW, float halfH, float x, float y, float zmin, float zmax);
