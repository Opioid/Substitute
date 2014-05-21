#pragma once

#include "Math/Matrix.hpp"
#include <cstdint>
#include <string>
#include <ostream>

namespace string
{

std::string to_string(bool value);
std::string to_string(int value, int num_padded_zeros = 0);

std::string to_string(uint32_t value, int num_padded_zeros = 0);
std::string to_string(uint64_t value, int num_padded_zeros = 0);

std::string to_string_short(float value);

std::string to_string(const float2& v);
std::string to_string(const uint2& v);
std::string to_string(const float3& v);

std::string format_byte_size(size_t value);

bool ends_with(const std::string& full_string, const std::string& ending);

}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const tVector2<T>& v)
{
	return stream << "[" << v.x << ", " << v.y << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const tVector3<T>& v)
{
	return stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const tVector4<T>& v)
{
	return stream << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const tMatrix4x4<T>& m)
{
	return stream << "[" << m.m00 << ", " << m.m01 << ", " <<  m.m02 << ", " <<  m.m03 << "," << std::endl
				  << " " << m.m10 << ", " << m.m11 << ", " <<  m.m12 << ", " <<  m.m13 << "," << std::endl
				  << " " << m.m20 << ", " << m.m21 << ", " <<  m.m22 << ", " <<  m.m23 << "," << std::endl
				  << " " << m.m30 << ", " << m.m31 << ", " <<  m.m32 << ", " <<  m.m33 << "]";
}
