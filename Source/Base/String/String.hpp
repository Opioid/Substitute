#pragma once

#include "Math/Vector.hpp"
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
std::ostream& operator<<(std::ostream& stream, const tVector2<T>& vector)
{
	return stream << "[" << vector.x << ", " << vector.y << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const tVector3<T>& vector)
{
	return stream << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const tVector4<T>& vector)
{
	return stream << "[" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "]";
}
