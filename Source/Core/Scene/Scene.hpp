#pragma once

#include "Camera.hpp"
#include "Actor.hpp"
//#include "Decal/Decal_provider.hpp"
#include "Light/Light.hpp"
#include "Light/Light_probe.hpp"
#include "AABB_tree/AABB_tree.hpp"
#include "Surrounding.hpp"
#include "Complex/Complex_factory_manager.hpp"
#include "Memory/Heap_cache.hpp"
#include <vector>

namespace scene
{

class Entity;
class Transformation_interpolator;
class Static_prop;
class Irradiance_volume;

class Scene
{

public:

	Scene();

	void release();

    void clear();

    void compile();

	const std::string& name() const;
	void set_name(const std::string& name);

	void on_tick(float time_slice);
	void update(float delta);

	const Camera& camera() const;
	Camera& camera();

	const AABB_tree& aabb_tree() const;
	AABB_tree& aabb_tree();

	Entity* entity(const std::string& name) const;

	const std::vector<Entity*>& entities() const;

	const Heap_cache<Actor>& actors() const;

//	const Decal_provider& get_decal_provider() const;

	const Heap_cache<Light>& lights() const;

	const Heap_cache<Irradiance_volume>& irradiance_volumes() const;

	const Light_probe* surrounding_light_probe() const;
	Light_probe* surrounding_light_probe();

	const Heap_cache<Light_probe>& light_probes() const;

	Actor* create_actor(bool interpolated = true, const std::string& name = "");
	Static_prop* create_static_prop();

	Light* create_light(Light::Type type);
	Irradiance_volume* create_irradiance_volume(const uint3& resolution, const float3& position, const float3& scale, const Quaternion& rotation);
	Light_probe* create_light_probe();
	Light_probe* create_light_probe(const float3& position, const float3& scale, const Quaternion& rotation);

	const Surrounding& surrounding() const;
	Surrounding& surrounding();

	Complex* create_complex(const std::string& type, Resource_manager& resource_manager);

	Complex_factory_manager& complex_factories();

private:

	std::string name_;

	Camera camera_;

	AABB_tree aabb_tree_;

	std::vector<Entity*> entities_;
	std::map<std::string, Entity*> named_entities_;

    Heap_cache<Actor> actors_;

	std::vector<Transformation_interpolator> interpolators_;

	Heap_cache<Static_prop> static_props_;

//	Decal_provider decal_provider_;

    Heap_cache<Light> lights_;

	Heap_cache<Irradiance_volume> irradiance_volumes_;

    bool surrounding_light_probe_valid_;
    Light_probe surrounding_light_probe_;

    Heap_cache<Light_probe> light_probes_;

	Surrounding surrounding_;

	Complex_factory_manager m_complex_factories;
	std::vector<Complex*> complexes_;
};

}


