#include "scripting_Entity.hpp"
#include "Script_tool.hpp"
#include "Scripting/Script_engine.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Light/Light.hpp"
#include <angelscript.h>

namespace scripting
{

template<typename Type>
bool register_node(const std::string& type_name, const Script_engine& engine)
{
	engine.register_object_type(type_name, 0, asOBJ_REF | asOBJ_NOCOUNT);

	engine.register_object_method(type_name, "const float3& local_position() const", asMETHODPR(Type, local_position, () const, const float3&));
	engine.register_object_method(type_name, "void set_local_position(const float3 &in)", asMETHODPR(Type, set_local_position, (const float3&), void));

	engine.register_object_method(type_name, "const float3& local_scale() const", asMETHODPR(Type, local_scale, () const, const float3&));
	engine.register_object_method(type_name, "void set_local_scale(const float3 &in)", asMETHODPR(Type, set_local_scale, (const float3&), void));

	engine.register_object_method(type_name, "const Quaternion& local_rotation() const", asMETHODPR(Type, local_rotation, () const, const Quaternion&));
	engine.register_object_method(type_name, "void set_local_rotation(const Quaternion &in)", asMETHODPR(Type, set_local_rotation, (const Quaternion&), void));

	engine.register_object_method(type_name, "float3 world_position() const", asMETHODPR(Type, world_position, () const, float3));
	engine.register_object_method(type_name, "float3 world_scale() const", asMETHODPR(Type, world_scale, () const, float3));

	engine.register_object_method(type_name, "float3 world_direction() const", asMETHODPR(Type, world_direction, () const, float3));
	engine.register_object_method(type_name, "float3 world_right() const", asMETHODPR(Type, world_right, () const, float3));
	engine.register_object_method(type_name, "float3 world_up() const", asMETHODPR(Type, world_up, () const, float3));

	return true;
}

bool init_entity(Script_tool& tool)
{
	const Script_engine& engine = tool.engine();

	engine.set_default_namespace("");

	if (!register_node<scene::Entity>("Entity", engine))
	{
		return false;
	}

	if (!register_node<scene::Entity>("Actor", engine))
	{
		return false;
	}

	if (!register_node<scene::Light>("Light", engine))
	{
		return false;
	}

	engine.register_object_method("Light", "const float3& color() const", asMETHODPR(scene::Light, color, () const, const float3&));
	engine.register_object_method("Light", "void set_color(const float3 &in)", asMETHODPR(scene::Light, set_color, (const float3&), void));

	engine.register_object_method("Light", "float lumen() const", asMETHODPR(scene::Light, lumen, () const, float));
	engine.register_object_method("Light", "void set_lumen(float)", asMETHODPR(scene::Light, set_lumen, (float), void));

	engine.register_object_method("Light", "float fov() const", asMETHODPR(scene::Light, fov, () const, float));
	engine.register_object_method("Light", "void set_fov(float)", asMETHODPR(scene::Light, set_fov, (float), void));

	return true;
}

}
