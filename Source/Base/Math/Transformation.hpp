#pragma once

template<typename T> struct Vector3;
template<typename T> struct Quaternion;

template<typename T>
struct tTransformation
{
	tTransformation()
	{}

	tTransformation(const Vector3<T>& position, const Vector3<T>& scale, const Quaternion<T>& rotation) :
		position(position), scale(scale), rotation(rotation)
	{}

	Vector3<T>   position;
	Vector3<T>   scale;
	Quaternion<T> rotation;

	static const tTransformation identity;
};

template<typename T>
const tTransformation<T> tTransformation<T>::identity = tTransformation<T>(
	Vector3<T>(0.f, 0.f, 0.f), Vector3<T>(1.f, 1.f, 1.f), Quaternion<T>(0.f, 0.f, 0.f, 1.f));

typedef tTransformation<float> Transformation;
