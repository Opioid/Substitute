#pragma once

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include <rapidjson/document.h>
#include <istream>

namespace json
{

bool parse(rapidjson::Document& document, std::istream& stream);

std::string get_error(rapidjson::Document& document, std::istream& stream);

float2 get_float2(const rapidjson::Value& value);
float3 get_float3(const rapidjson::Value& value);
float4 get_float4(const rapidjson::Value& value);

uint3  get_uint3(const rapidjson::Value& value);

Quaternion get_quaternion(const rapidjson::Value& value);
float3x3   get_rotation_matrix(const rapidjson::Value& value);
Quaternion get_local_rotation(const rapidjson::Value& value);

bool get_bool(const rapidjson::Value& value, const std::string& name, bool default_value);
std::string get_string(const rapidjson::Value& value, const std::string& name, const std::string& default_value);
uint32_t get_uint(const rapidjson::Value& value, const std::string& name, uint32_t default_value);

}
