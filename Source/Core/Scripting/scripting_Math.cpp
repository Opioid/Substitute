#include "scripting_Math.hpp"
#include "Script_tool.hpp"
#include "Math/Math.hpp"
#include "Math/Vector.hpp"
#include "Logging/Logging.hpp"
#include "String/String.hpp"
#include <angelscript.h>

namespace scripting
{
	void init_constructor(float x, float y, float2* self);
	void init_constructor(int x, int y, int2* self);
	void init_constructor(uint32_t x, uint32_t y, uint2* self);
	void init_constructor(float x, float y, float z, float3* self);

	void print(const float2& value);
	void print(int2   const& value);
	void print(uint2  const& value);
	void print(const float3& value);

	float min(float a, float b);
	float max(float a, float b);

	struct floaty2
	{
		float x;
		float y;

		floaty2(float x, float y) : x(x), y(y)
		{}
	};

	void floaty2_init_constructor(float x, float y, floaty2* self)
	{
		new(self) floaty2(x, y);
	}

	bool init_math(Script_tool& tool)
	{
		Script_engine const& engine = tool.get_engine();

		engine.set_default_namespace("");

		engine.register_variable("const float pi", (float*)&math::pi);

		engine.register_function("float sin(float)", asFUNCTIONPR(std::sin, (float), float));
		engine.register_function("float cos(float)", asFUNCTIONPR(std::cos, (float), float));

		engine.register_function("float min(float, float)", asFUNCTIONPR(min, (float, float), float));
		engine.register_function("float max(float, float)", asFUNCTIONPR(max, (float, float), float));

		engine.register_function("float random(float, float)", asFUNCTIONPR(math::random, (float, float), float));

		//float2----------------------------------------------
		engine.register_object_type("float2", sizeof(float2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS);

		engine.register_object_property("float2", "float x", offsetof(float2, x)); 
		engine.register_object_property("float2", "float y", offsetof(float2, y)); 

		engine.register_object_behavior("float2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTIONPR(init_constructor, (float, float, float2*), void), Script_engine::Calling_convention::C_decl_obj_last);

		engine.register_function("void print(const float2 &in)", asFUNCTIONPR(print, (const float2&), void));

		//int2----------------------------------------------
		engine.register_object_type("int2", sizeof(int2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLINTS);

		engine.register_object_property("int2", "int x", offsetof(int2, x)); 
		engine.register_object_property("int2", "int y", offsetof(int2, y)); 

		engine.register_object_behavior("int2", asBEHAVE_CONSTRUCT, "void f(int, int)", asFUNCTIONPR(init_constructor, (int, int, int2*), void), Script_engine::Calling_convention::C_decl_obj_last);

		engine.register_function("void print(const int2 &in)", asFUNCTIONPR(print, (const int2&), void));

		//uint2----------------------------------------------
		engine.register_object_type("uint2", sizeof(uint2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLINTS);

		engine.register_object_property("uint2", "uint x", offsetof(uint2, x)); 
		engine.register_object_property("uint2", "uint y", offsetof(uint2, y)); 

		engine.register_object_behavior("uint2", asBEHAVE_CONSTRUCT, "void f(uint, uint)", asFUNCTIONPR(init_constructor, (uint32_t, uint32_t, uint2*), void), Script_engine::Calling_convention::C_decl_obj_last);

		engine.register_function("void print(const uint2 &in)", asFUNCTIONPR(print, (const uint2& ), void));

		//float3----------------------------------------------
		engine.register_object_type("float3", sizeof(float3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS);

		engine.register_object_property("float3", "float x", offsetof(float3, x)); 
		engine.register_object_property("float3", "float y", offsetof(float3, y)); 
		engine.register_object_property("float3", "float z", offsetof(float3, z)); 

		engine.register_object_property("float3", "float r", offsetof(float3, r)); 
		engine.register_object_property("float3", "float g", offsetof(float3, g)); 
		engine.register_object_property("float3", "float b", offsetof(float3, b)); 

		engine.register_object_behavior("float3", asBEHAVE_CONSTRUCT, "void f(float, float, float)",  asFUNCTIONPR(init_constructor, (float, float, float, float3 *), void), Script_engine::Calling_convention::C_decl_obj_last);
/*
		engine.register_object_method("float3", "float3 opAdd(const float3 &in) const", asMETHODPR(float3, operator+, (const float3&) const, float3));
		engine.register_object_method("float3", "float3 opSub(const float3 &in) const", asMETHODPR(float3, operator-, (const float3&) const, float3));
		engine.register_object_method("float3", "float3 opMul_r(float) const", asFUNCTIONPR(operator*, (float, const float3&), float3), Script_engine::Calling_convention::C_decl_obj_last);
		engine.register_object_method("float3", "float3 opDiv(float) const", asMETHODPR(float3, operator/, (float) const, float3));
		
		engine.register_object_method("float3", "float3 opNeg()", asMETHODPR(float3, operator-, () const, float3));
*/
		engine.register_object_method("float3", "float3 &opMulAssign(float)", asMETHODPR(float3, operator*=, (float), float3&));
		engine.register_object_method("float3", "float3 &opDivAssign(float)", asMETHODPR(float3, operator/=, (float), float3&));
		engine.register_object_method("float3", "float3 &opAddAssign(const float3 &in)", asMETHODPR(float3, operator+=, (const float3&), float3&));
		engine.register_object_method("float3", "float3 &opSubAssign(const float3 &in)", asMETHODPR(float3, operator-=, (const float3&), float3&));

		engine.register_function("float dot(const float3 &in, const float3 &in)", asFUNCTIONPR(dot, (const float3&, const float3&), float));
		engine.register_function("float length(const float3 &in)", asFUNCTIONPR(length, (const float3&), float));
		engine.register_function("float3 normalize(const float3 &in)", asFUNCTIONPR(normalize, (const float3&), float3));
		engine.register_function("float distance(const float3 &in, const float3 &in)", asFUNCTIONPR(distance, (const float3&, const float3&), float));

		engine.register_function("void print(const float3 &in)", asFUNCTIONPR(print, (const float3&), void));

		return true;
	}

	void init_constructor(float x, float y, float2* self)
	{
		new(self) float2(x, y);
	}

	void init_constructor(int x, int y, int2* self)
	{
		new(self) int2(x, y);
	}

	void init_constructor(uint32_t x, uint32_t y, uint2* self)
	{
		new(self) uint2(x, y);
	}

	void init_constructor(float x, float y, float z, float3* self)
	{
		new(self) float3(x, y, z);
	}

	void print(const float2& value)
	{
		logging::post("[" + string::to_string_short(value.x) + ", " + string::to_string_short(value.y) + "]");
	}

	void print(const int2& value)
	{
		logging::post("[" + string::to_string(value.x) + ", " + string::to_string(value.y) + "]");
	}

	void print(const uint2& value)
	{
		logging::post("[" + string::to_string(value.x) + ", " + string::to_string(value.y) + "]");
	}

	void print(const float3& value)
	{
		logging::post("[" + string::to_string_short(value.x) + ", " + string::to_string_short(value.y) + ", " + string::to_string_short(value.z) + "]");
	}

	float min(float a, float b)
	{
		return std::min(a, b);
	}

	float max(float a, float b)
	{
		return std::max(a, b);
	}
}
