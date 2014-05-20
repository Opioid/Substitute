#include "String.hpp"
#include "Math/Math.hpp"
#include <sstream>
#include <iomanip>

namespace string
{

std::string to_string(bool value)
{
	return value ? "true" : "false";
}

std::string to_string(int value, int num_padded_zeros)
{
	std::ostringstream ost;

	if (num_padded_zeros)
	{
		ost << std::setw(num_padded_zeros + 1) << std::setfill('0');
	}

	ost << value;
	return ost.str();
}

std::string to_string(uint32_t value, int num_padded_zeros)
{
	std::ostringstream ost;

	if (num_padded_zeros)
	{
		ost << std::setw(num_padded_zeros + 1) << std::setfill('0');
	}

	ost << value;
	return ost.str();
}

std::string to_string(uint64_t value, int num_padded_zeros)
{
	std::ostringstream ost;

	if (num_padded_zeros)
	{
		ost << std::setw(num_padded_zeros + 1) << std::setfill('0');
	}

	ost << value;
	return ost.str();
}

std::string to_string_short(float value)
{
	std::ostringstream ost;
	ost.precision(6);
	ost << value;
	return ost.str();
}

std::string to_string(const float2& v)
{
    std::ostringstream ost;
    ost << "[" << v.x << ", " << v.y << "]";
    return ost.str();
}

std::string to_string(const uint2& v)
{
    std::ostringstream ost;
    ost << "[" << v.x << ", " << v.y << "]";
    return ost.str();
}

std::string to_string(const float3& v)
{
	std::ostringstream ost;
	ost << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	return ost.str();
}

std::string format_byte_size(size_t value)
{
	std::ostringstream ost;

	if (value < 1024)
	{
		ost << value;
		ost << " b";
	}
	else if (value < 1024 * 1024)
	{
		ost << math::round(float(value) / 1024.f * 100.f) / 100.f;
		ost << " kb";
	}
	else if (value < 1024 * 1024 * 1024)
	{
		ost << math::round(float(value) / (1024.f * 1024.f) * 100.f) / 100.f;
		ost << " mb";
	}

	return ost.str();
}

bool ends_with(const std::string& full_string, const std::string& ending)
{
	if (full_string.length() >= ending.length())
	{
		return 0 == full_string.compare(full_string.length() - ending.length(), ending.length(), ending);
	}
	else
	{
		return false;
	}
}

}


