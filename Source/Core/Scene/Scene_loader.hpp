#pragma once

#include "Parsing/Rapidjson_types.hpp"
#include <string>

class Resource_manager;

namespace rendering
{

class Rendering_tool;

}

namespace scene
{

class Scene;
class Entity;

class Scene_loader
{

public:

	Scene_loader(Scene& scene, rendering::Rendering_tool& rendering_tool, Resource_manager& resource_manager);

    bool load(const std::string& name);

private:

	void load_camera(const rapidjson::Value& camera_value);
	void load_surrounding(const rapidjson::Value& surrounding);
	void load_entities(const rapidjson::Value& entities, Entity* parent = nullptr);

	Entity* load_light(const rapidjson::Value& entity);
	Entity* load_actor(const rapidjson::Value& entity);
	Entity* load_complex(const rapidjson::Value& entity);

	void load_irradiance_volumes(const rapidjson::Value& volumes);
	void load_irradiance_volume(const rapidjson::Value& volume);

	void load_light_probes(const rapidjson::Value& probes);
	void load_light_probe(const rapidjson::Value& probe);

	void load_static_props(const rapidjson::Value& static_props);
	void load_static_prop(const rapidjson::Value& static_prop);

	void load_abt(const rapidjson::Value& abt);

	Scene& scene_;
	rendering::Rendering_tool& rendering_tool_;
	Resource_manager& resource_manager_;
};

}
