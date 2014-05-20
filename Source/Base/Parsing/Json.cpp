#include "Json.hpp"
#include "Json_stream.hpp"
#include <rapidjson/document.h>
#include <sstream>

namespace json
{

bool parse(rapidjson::Document& document, std::istream& stream)
{
    json::Input_stream json_stream(stream);

    document.ParseStream<0, rapidjson::UTF8<>>(json_stream);

	return document.HasParseError() == false;
}

size_t get_line_number(std::istream& stream, size_t offset);

std::string get_error(rapidjson::Document& document, std::istream& stream)
{
	size_t line = get_line_number(stream, document.GetErrorOffset());

	std::stringstream sstream;

	sstream << document.GetParseError();
	// line number is 0 based, so + 1
	sstream << " (line " << line + 1 << ")";

	return sstream.str();
}

// get the 0-based line number
size_t get_line_number(std::istream& stream, size_t offset)
{
	size_t line = 0;
	size_t count = 0;

	stream.clear();
	stream.seekg(0, std::ios_base::beg);

	char c = 0;

	while (stream && count < offset)
	{
		stream.get(c);

		++count;

		if ('\n' == c)
		{
			++line;
		}
	}

	return line;
}

float2 get_float2(const rapidjson::Value& value)
{
	return float2(float(value[0u].GetDouble()), float(value[1].GetDouble()));
}

float3 get_float3(const rapidjson::Value& value)
{
	return float3(float(value[0u].GetDouble()), float(value[1].GetDouble()), float(value[2].GetDouble()));
}

float4 get_float4(const rapidjson::Value& value)
{
	return float4(float(value[0u].GetDouble()), float(value[1].GetDouble()), float(value[2].GetDouble()), float(value[3].GetDouble()));
}

uint3 get_uint3(const rapidjson::Value& value)
{
    return uint3(value[0u].GetUint(), value[1].GetUint(), value[2].GetUint());
}

Quaternion get_quaternion(const rapidjson::Value& value)
{
	 return Quaternion(float(value[0u].GetDouble()), float(value[1].GetDouble()), float(value[2].GetDouble()), float(value[3].GetDouble()));
}

float3x3 get_rotation_matrix(const rapidjson::Value& value)
{
    float3 rot = get_float3(value);

    float3x3 rot_x;
    set_rotation_x(rot_x, math::to_radians(rot.x));

    float3x3 rot_y;
    set_rotation_y(rot_y, math::to_radians(rot.y));

    float3x3 rot_z;
    set_rotation_z(rot_z, math::to_radians(rot.z));

    return rot_z * rot_x * rot_y;
}

Quaternion get_local_rotation(const rapidjson::Value& value)
{
    return Quaternion(get_rotation_matrix(value));
}

bool get_bool(const rapidjson::Value& value, const std::string& name, bool default_value)
{
	const rapidjson::Value::Member* node = value.FindMember(name.c_str());

	if (!node)
	{
		return default_value;
	}

	return node->value.GetBool();
}

std::string get_string(const rapidjson::Value& value, const std::string& name, const std::string& default_value)
{
	const rapidjson::Value::Member* node = value.FindMember(name.c_str());

	if (!node)
	{
		return default_value;
	}

	return node->value.GetString();
}

uint32_t get_uint(const rapidjson::Value& value, const std::string& name, uint32_t default_value)
{
	const rapidjson::Value::Member* node = value.FindMember(name.c_str());

	if (!node)
	{
		return default_value;
	}

	return node->value.GetUint();
}

}