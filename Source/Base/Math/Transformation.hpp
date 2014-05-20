#pragma once

template<typename T> struct tVector3;
template<typename T> struct tQuaternion;

template<typename T>
struct tTransformation
{
	tTransformation()
	{}

	tTransformation(const tVector3<T>& position, const tVector3<T>& scale, const tQuaternion<T>& rotation)
		: position(position), scale(scale), rotation(rotation)
	{}

	tVector3<T>    position;
	tVector3<T>    scale;
	tQuaternion<T> rotation;

	static const tTransformation identity;
};

template<typename T>
const tTransformation<T> tTransformation<T>::identity = tTransformation<T>(
	tVector3<T>(0.f, 0.f, 0.f), tVector3<T>(1.f, 1.f, 1.f), tQuaternion<T>(0.f, 0.f, 0.f, 1.f));

typedef tTransformation<float> Transformation;
