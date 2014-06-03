#include "Scene.hpp"
#include "Model.hpp"
#include "Actor.hpp"
#include "Static_prop.hpp"
#include "Transformation_interpolator.hpp"
#include "Light/Irradiance_volume.hpp"
#include "AABB_tree/AABB_tree_builder.hpp"
#include "Particles/Particle_effect.hpp"
#include "Complex/Complex.hpp"
#include "Math/Quaternion.inl"

namespace scene
{

Scene::Scene() : surrounding_light_probe_valid_(false)
{}

void Scene::release()
{
    clear();
}

void Scene::clear()
{
	// clear actors
	for (auto a : actors_)
	{
        a->clear();
	}

	actors_.clear();

	// clear lights
	for (auto l : lights_)
	{
        l->clear();
	}

	lights_.clear();

	entities_.clear();

	named_entities_.clear();

	// clear irradiance volumes
	for (auto iv : irradiance_volumes_)
	{
		iv->clear();
	}

	irradiance_volumes_.clear();

	surrounding_light_probe_valid_ = false;
	surrounding_light_probe_.clear();

	// clear light probes
	for (auto lp : light_probes_)
	{
        lp->clear();
	}

	light_probes_.clear();

	// clear static props
	for (auto p : static_props_)
	{
        p->clear();
	}

	static_props_.clear();

	for (auto c : complexes_)
	{
		delete c;
	}

	complexes_.clear();

	surrounding_.clear();

	aabb_tree_.clear();
}

void Scene::compile()
{
	for (auto e : entities_)
	{
		if (!e->is_interpolated())
		{
			e->fix_world_transformation();
		}
	}

	for (auto& i : interpolators_)
	{
		i.copy_previous_state();
	}

	if (aabb_tree_.is_empty())
	{
		AABB_tree_builder tree_builder;
		tree_builder.build(aabb_tree_, static_props_);
	}
	else
	{
		for (auto p : static_props_)
		{
			aabb_tree_.add_static_prop(p);
		}
	}

	if (!aabb_tree_.is_empty())
    {
		if (irradiance_volumes_.empty())
        {
			const AABB& aabb = aabb_tree_.root()->aabb();

            // make the volume slightly larger than the area to cover to avoid z-fighting when the volume is rendered
			create_irradiance_volume(uint3(2, 2, 2), aabb.position, aabb.halfsize + float3(0.01f, 0.01f, 0.01f), quaternion::identity);
        }
		else if (irradiance_volumes_[0]->is_faulty())
		{
			const AABB& aabb = aabb_tree_.root()->aabb();

			irradiance_volumes_[0]->adjust(aabb.position, aabb.halfsize + float3(0.01f, 0.01f, 0.01f));
		}

		if (light_probes_.empty())
        {
            create_light_probe();
        }
		else if (light_probes_[0]->is_faulty())
		{
			const AABB& aabb = aabb_tree_.root()->aabb();

			light_probes_[0]->adjust(aabb.position, aabb.halfsize + float3(0.01f, 0.01f, 0.01f));
		}
    }
}

const std::string& Scene::name() const
{
	return name_;
}

void Scene::set_name(const std::string& name)
{
	name_ = name;
}

void Scene::on_tick(float time_slice)
{
	for (auto& i : interpolators_)
	{
		i.copy_previous_state();
	}

	for (auto c : complexes_)
	{
		c->on_tick(time_slice);
	}

	particle_scene_.on_tick(time_slice);
}

void Scene::update(float delta)
{
	for (auto& i : interpolators_)
	{
		i.interpolate(delta);
	}

	for (auto e : entities_)
	{
		e->propagate_transformation();
	}

	for (auto a : actors_)
	{
		a->on_finalized_world_transformation();
	}

	camera_.update_view();
}

const Camera& Scene::camera() const
{
	return camera_;
}

Camera& Scene::camera()
{
	return camera_;
}

const AABB_tree& Scene::aabb_tree() const
{
	return aabb_tree_;
}

AABB_tree& Scene::aabb_tree()
{
	return aabb_tree_;
}

Entity* Scene::entity(const std::string& name) const
{
	auto e = named_entities_.find(name);

	if (named_entities_.end() == e)
	{
		return nullptr;
	}

	return e->second;
}

Actor* Scene::actor(const std::string& name) const
{
	return static_cast<Actor*>(entity(name));
}

Light* Scene::light(const std::string& name) const
{
	return static_cast<Light*>(entity(name));
}

const std::vector<Entity*>& Scene::entities() const
{
	return entities_;
}

const Heap_cache<Actor>& Scene::actors() const
{
	return actors_;
}

//const Decal_provider& Scene::get_decal_provider() const
//{
//	return decal_provider_;
//}

const Heap_cache<Light>& Scene::lights() const
{
	return lights_;
}

const Heap_cache<Irradiance_volume>& Scene::irradiance_volumes() const
{
	return irradiance_volumes_;
}

const Light_probe* Scene::surrounding_light_probe() const
{
	return surrounding_light_probe_valid_ ? &surrounding_light_probe_ : nullptr;
}

Light_probe* Scene::surrounding_light_probe()
{
	return surrounding_light_probe_valid_ ? &surrounding_light_probe_ : nullptr;
}

const Heap_cache<Light_probe>& Scene::light_probes() const
{
	return light_probes_;
}

Actor* Scene::create_actor(bool interpolated, const std::string& name)
{
	Actor* actor = actors_.add();

	add_entity(actor, name);

	if (interpolated)
	{
		interpolators_.push_back(Transformation_interpolator(actor));
	}

	return actor;
}

Static_prop* Scene::create_static_prop()
{
	return static_props_.add();
}

Light* Scene::create_light(Light::Type type, const std::string& name)
{
	Light* light = lights_.add();

	add_entity(light, name);

    light->init(type);

	return light;
}

Irradiance_volume* Scene::create_irradiance_volume(const uint3& resolution, const float3& position, const float3& scale, const quaternion& rotation)
{
	Irradiance_volume* irradiance_volume = irradiance_volumes_.add();

	irradiance_volume->init(resolution, position, scale, rotation);

	return irradiance_volume;
}

Light_probe* Scene::create_light_probe()
{
	surrounding_light_probe_valid_ = true;

	return &surrounding_light_probe_;
}

Light_probe* Scene::create_light_probe(const float3& position, const float3& scale, const quaternion& rotation)
{
	Light_probe* light_probe = light_probes_.add();

    light_probe->init(position, scale, rotation);

	return light_probe;
}

Particle_effect* Scene::create_particle_effect(const std::string& type, bool interpolated, const std::string& name)
{
	Particle_effect* particle_effect = particle_scene_.create_particle_effect(type);

	if (!particle_effect)
	{
		return nullptr;
	}

	add_entity(particle_effect, name);

	interpolators_.push_back(Transformation_interpolator(particle_effect));

	return particle_effect;
}

const Particle_scene& Scene::particle_scene() const
{
	return particle_scene_;
}

Particle_scene& Scene::particle_scene()
{
	return particle_scene_;
}

const scene::Surrounding& Scene::surrounding() const
{
	return surrounding_;
}

scene::Surrounding& Scene::surrounding()
{
	return surrounding_;
}

Complex* Scene::create_complex(const std::string& type, Resource_manager& resource_manager)
{
	Complex* complex = complex_factories_.create_complex(type, *this, resource_manager);

	if (!complex)
	{
		return nullptr;
	}

	complexes_.push_back(complex);

	return complex;
}

Complex_factory_manager& Scene::complex_factories()
{
	return complex_factories_;
}

void Scene::add_entity(Entity* entity, const std::string& name)
{
	entities_.push_back(entity);

	if (!name.empty())
	{
		named_entities_[name] = entity;
	}

	entity->set_visible(true);
}

}
