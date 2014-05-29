#include "Scene_loader.hpp"
#include "File/File_stream.hpp"
#include "Scene.hpp"
#include "Model.hpp"
#include "Static_prop.hpp"
#include "AABB_tree/AABB_tree_loader.hpp"
#include "Complex/Complex.hpp"
#include "Scripting/Scripter.hpp"
#include "Resources/Resource_manager.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Texture_provider.hpp"
#include "Timing/Stopwatch.hpp"
#include "Parsing/Json.hpp"
#include "Logging/Logging.hpp"

namespace scene
{

Scene_loader::Scene_loader(Scene &scene, scripting::Scripter& scripter, rendering::Rendering_tool& rendering_tool, Resource_manager& resource_manager) :
	scene_(scene), scripter_(scripter), rendering_tool_(rendering_tool), resource_manager_(resource_manager)
{}

bool Scene_loader::load(const std::string& name)
{
	timing::Stopwatch stopwatch;
	stopwatch.start();

    scene_.clear();

	file::Input_stream stream(name);

	if (!stream)
	{
		logging::error("Scene_loader::load(): could not open stream for \"" + name + "\".");

		return false;
	}

	rapidjson::Document root;

	if (!json::parse(root, stream))
	{
		logging::error("Scene_loader::load(): \"" + name + "\" could not be loaded: failed to parse the file: " + json::read_error(root, stream));

		return false;
	}

	scripter_.start();

	auto& file_system = resource_manager_.virtual_file_system();

	std::string resolved_path;
	file_system.query_resolved_complete_path(resolved_path, name);
	file_system.push_mount(resolved_path);

	for (auto n = root.MemberBegin(); n != root.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("camera" == node_name)
		{
			load_camera(node_value);
		}
		else if ("surrounding" == node_name)
		{
			load_surrounding(node_value);
		}
		else if ("entities" == node_name)
		{
			load_entities(node_value);
		}
		else if ("static_props" == node_name)
		{
			load_static_props(node_value);
		}
		else if ("abt" == node_name)
		{
			load_abt(node_value);
		}
		else if ("irradiance_volumes" == node_name)
		{
			load_irradiance_volumes(node_value);
		}
		else if ("light_probes" == node_name)
		{
			load_light_probes(node_value);
		}
		else if ("scripting" == node_name)
		{
			Scripting scripting;
			load_scripting(node_value, scripting);

			scripter_.register_script_class(scene_, scripting.file_name, scripting.class_name);
		}
	}

	scripter_.compile();

	file_system.pop_mount();

	scene_.set_name(name);

	logging::post("Scene \"" + name + "\" loaded (" + string::to_string_short(float(stopwatch.stop())) + " s).");

	return true;
}

void Scene_loader::load_camera(const rapidjson::Value& camera_value)
{
	auto& camera = scene_.camera();

	for (auto n = camera_value.MemberBegin(); n != camera_value.MemberEnd(); ++n)
	{ 
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("position" == node_name)
		{
			camera.set_local_position(json::read_float3(node_value));
		}
		else if ("exposure" == node_name)
		{
			camera.set_exposure(float(node_value.GetDouble()));
		}
		else if ("linear_white" == node_name)
		{
			float linear_white = float(node_value.GetDouble());
			camera.set_linear_white(float3(linear_white, linear_white, linear_white));
		}
	}
}

void Scene_loader::load_surrounding(const rapidjson::Value& surrounding)
{
	for (auto n = surrounding.MemberBegin(); n != surrounding.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "color")
		{
			scene_.surrounding().set_color(json::read_float3(node_value));
		}
		else if (node_name == "texture")
		{
			Flags flags;
			flags.set(rendering::Texture_provider::Options::Texture_Cube, true);

			scene_.surrounding().set_texture(resource_manager_.load<rendering::Shader_resource_view>(node_value.GetString(), flags));
		}
	}
}

void Scene_loader::load_entities(const rapidjson::Value& entities, Entity* parent)
{
	if (!entities.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < entities.Size(); ++i)
	{
		const rapidjson::Value& value = entities[i];

		const rapidjson::Value::Member* class_node = value.FindMember("class");

		if (!class_node)
		{
			continue;
		}

		Entity* entity = nullptr;

		std::string class_name = class_node->value.GetString();

		if ("Light" == class_name)
		{
			entity = load_light(value);
		}
		else if ("Actor" == class_name)
		{
			entity = load_actor(value);
		}
		else if ("Complex" == class_name)
		{
			entity = load_complex(value);
		}

		if (entity)
		{
			float3 position(float3::identity);
			float3 scale(1.f, 1.f, 1.f);
			Quaternion rotation(Quaternion::identity);

			for (auto n = value.MemberBegin(); n != value.MemberEnd(); ++n)
			{
				const std::string property_name = n->name.GetString();
				const rapidjson::Value& property_value = n->value;

				if ("position" == property_name)
				{
					position = json::read_float3(property_value);
				}
				else if ("scale" == property_name)
				{
					scale = json::read_float3(property_value);
				}
				else if ("rotation" == property_name)
				{
					rotation = json::read_local_rotation(property_value);
				}				
			}

			entity->set_local_position(position);
			entity->set_local_scale(scale);
			entity->set_local_rotation(rotation);

			const rapidjson::Value::Member* children = value.FindMember("entities");

			if (children)
			{
				load_entities(children->value, entity);
			}

			if (parent)
			{
				parent->attach(entity);
			}
		}
	}
}

Entity* Scene_loader::load_light(const rapidjson::Value& entity)
{
	const rapidjson::Value::Member* type = entity.FindMember("type");

	if (!type)
	{
		return nullptr;
	}

	const std::string type_name = type->value.GetString();

	const std::string name = json::read_string(entity, "name");

	Light* light = nullptr;

	if ("Directional" == type_name)
	{
		light = scene_.create_light(Light::Type::Directional, name);
	}
	else if ("Point" == type_name)
	{
		light = scene_.create_light(Light::Type::Point, name);
	}
	else if ("Spot" == type_name)
	{
		light = scene_.create_light(Light::Type::Spot, name);
	}

	if (light)
	{
		for (auto n = entity.MemberBegin(); n != entity.MemberEnd(); ++n)
		{
			const std::string node_name = n->name.GetString();
			const rapidjson::Value& node_value = n->value;

			if ("color" == node_name)
			{
				light->set_color(json::read_float3(node_value));
			}
			if ("lumen" == node_name)
			{
				light->set_lumen(static_cast<float>(node_value.GetDouble()));
			}
			else if ("shadow" == node_name)
			{
				light->set_casts_shadow(node_value.GetBool());
			}
			else if ("fov" == node_name)
			{
				light->set_fov(math::to_radians(static_cast<float>(node_value.GetDouble())));
			}
			else if ("texture" == node_name)
			{
				light->set_texture(resource_manager_.load<rendering::Shader_resource_view>(node_value.GetString()));
			}
			else if ("scripting" == node_name)
			{
				Scripting scripting;
				load_scripting(node_value, scripting);

				scripter_.register_script_class(*light, scripting.file_name, scripting.class_name);
			}
		}
	}

	return light;
}

Entity* Scene_loader::load_actor(const rapidjson::Value& entity)
{
	std::string name;
	Handle<Model> model;
	std::vector<Handle<Material>> materials; 

	for (auto n = entity.MemberBegin(); n != entity.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("name" == node_name)
		{
			name = node_value.GetString();
		}
		else if ("model" == node_name)
		{
			model = resource_manager_.load<Model>(node_value.GetString());
		}
		else if ("materials" == node_name)
		{
			if (!node_value.IsArray())
			{
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				const rapidjson::Value& material_value = node_value[i];

				Handle<Material> material = resource_manager_.load<Material>(material_value.GetString());

				if (material)
				{
					materials.push_back(material);
				}
			}
		}
	}

	if (!model || materials.empty())
	{
		return nullptr;
	}

	Actor* actor = scene_.create_actor(true, name);

	actor->create_surfaces(model, static_cast<uint32_t>(materials.size()), materials.data());

	return actor;
}

Entity* Scene_loader::load_complex(const rapidjson::Value& entity)
{
	const rapidjson::Value::Member* type = entity.FindMember("type");

	if (!type)
	{
		return nullptr;
	}

	const std::string type_name = type->value.GetString();

	return scene_.create_complex(type_name, resource_manager_);
}

void Scene_loader::load_irradiance_volumes(const rapidjson::Value& volumes)
{
	if (!volumes.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < volumes.Size(); ++i)
	{
		load_irradiance_volume(volumes[i]);
	}
}

void Scene_loader::load_irradiance_volume(const rapidjson::Value& volume)
{
	uint3 resolution(2, 2, 2);
	float3 position(float3::identity);
	float3 scale(float3::identity);
	Quaternion rotation(Quaternion::identity);

	for (auto n = volume.MemberBegin(); n != volume.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "resolution")
		{
			resolution = json::read_uint3(node_value);
		}
		else if (node_name == "position")
		{
			position = json::read_float3(node_value);
		}
		else if (node_name == "scale")
		{
			scale = json::read_float3(node_value);
		}
		else if (node_name == "rotation")
		{
			rotation = json::read_local_rotation(node_value);
		}
	}

	scene_.create_irradiance_volume(resolution, position, scale, rotation);
}

void Scene_loader::load_light_probes(const rapidjson::Value& probes)
{
	if (!probes.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < probes.Size(); ++i)
	{
		load_light_probe(probes[i]);
	}
}

void Scene_loader::load_light_probe(const rapidjson::Value& probe)
{
	float3 position(float3::identity);
	float3 scale(float3::identity);
	Quaternion rotation(Quaternion::identity);

	for (auto n = probe.MemberBegin(); n != probe.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "position")
		{
			position = json::read_float3(node_value);
		}
		else if (node_name == "scale")
		{
			scale = json::read_float3(node_value);
		}
		else if (node_name == "rotation")
		{
			rotation = json::read_local_rotation(node_value);
		}
	}

    scene_.create_light_probe(position, scale, rotation);
}

void Scene_loader::load_static_props(const rapidjson::Value& static_props)
{
	if (!static_props.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < static_props.Size(); ++i)
	{
		load_static_prop(static_props[i]);
	}
}

void Scene_loader::load_static_prop(const rapidjson::Value& static_prop)
{
	Handle<Model> model;
	std::vector<Handle<Material>> materials; 

	float3 position(float3::identity);
	float3 scale(1.f, 1.f, 1.f);
	Quaternion rotation(Quaternion::identity);

	for (auto n = static_prop.MemberBegin(); n != static_prop.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "model")
		{
			model = resource_manager_.load<Model>(node_value.GetString());
		}
		else if (node_name == "materials")
		{
			if (!node_value.IsArray())
			{
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				const rapidjson::Value& material = node_value[i];

				materials.push_back(resource_manager_.load<Material>(material.GetString()));
			}
		}
		else if (node_name == "position")
		{
			position = json::read_float3(node_value);
		}
		else if (node_name == "scale")
		{
			scale = json::read_float3(node_value);
		}
		else if (node_name == "rotation")
		{
			rotation = json::read_local_rotation(node_value);
		}
	}

	if (!model || materials.empty() || !materials[0])
	{
		return;
	}

	Static_prop* prop = scene_.create_static_prop();

	prop->create_surfaces(model, static_cast<uint32_t>(materials.size()), materials.data());

	prop->set_world_transformation(position, scale, float3x3(rotation));
}

void Scene_loader::load_abt(const rapidjson::Value& abt)
{
	std::string abt_name;
	std::vector<Handle<Material>> materials; 

	for (auto n = abt.MemberBegin(); n != abt.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("abt" == node_name)
		{
			abt_name = node_value.GetString();
		}
		else if ("materials" == node_name)
		{
			if (!node_value.IsArray())
			{
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				const rapidjson::Value& material = node_value[i];

				materials.push_back(resource_manager_.load<Material>(material.GetString()));
			}
		}
	}

	if (abt_name.empty() || !materials.size() || !materials[0]) 
	{
		return;
	}

	file::Input_stream stream(abt_name);

	if (!stream)
	{
		logging::error("Scene_loader::load_atb(): \"" + abt_name + "\" could not be loaded: file not found.");
		return;
	}

	AABB_tree_loader loader(rendering_tool_);
	loader.load(stream, scene_.aabb_tree(), materials);
}

void Scene_loader::load_scripting(const rapidjson::Value& scripting_value, Scripting& scripting)
{
	for (auto n = scripting_value.MemberBegin(); n != scripting_value.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("source" == node_name)
		{
			scripting.file_name = node_value.GetString();
		}
		else if ("class" == node_name)
		{
			scripting.class_name = node_value.GetString();
		}
	}
}

}
