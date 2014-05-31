#include "Particle_scene.hpp"

namespace scene
{

Particle_effect* Particle_scene::create_particle_effect(const std::string& type)
{
	auto factory = factories_.find(type);

	if (factories_.end() != factory)
	{
		/*
		Complex* complex = factory->second->create_complex();

		if (!complex->init(scene, resource_manager))
		{
			delete complex;
			return nullptr;
		}

		return complex;
		*/
	}

	return nullptr;
}

void Particle_scene::register_factory(Particle_effect_factory* factory, const std::string& type)
{
	factories_[type] = factory;
}

}
