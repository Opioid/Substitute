#include "scripting_Entity.hpp"
#include "Script_tool.hpp"
#include "Scripting/Script_engine.hpp"
#include "Scene/Entity.hpp"
#include <angelscript.h>

namespace scripting
{

bool init_entity(Script_tool& tool)
{
	const Script_engine& engine = tool.engine();

	engine.set_default_namespace("");

	engine.register_object_type("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);

	engine.register_object_method("Entity", "const float3& local_position() const", asMETHODPR(scene::Entity, local_position, () const, const float3&));
	engine.register_object_method("Entity", "void set_local_position(const float3 &in)", asMETHODPR(scene::Entity, set_local_position, (const float3&), void));

	engine.register_object_method("Entity", "void set_local_rotation(const Quaternion &in)", asMETHODPR(scene::Entity, set_local_rotation, (const Quaternion&), void));

	return true;
}

}
