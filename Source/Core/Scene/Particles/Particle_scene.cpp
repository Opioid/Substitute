#include "Particle_scene.hpp"
#include "Particle_effect.hpp"
#include "Particle_effect_factory.hpp"

namespace scene
{

Particle_effect* Particle_scene::create_particle_effect(const std::string& type)
{
	auto factory = factories_.find(type);

	if (factories_.end() != factory)
	{
		Particle_effect* particle_effect = particle_effects_.add();

		if (!factory->second->init_particle_effect(*particle_effect))
		{
			particle_effects_.pop();

			return nullptr;
		}

		return particle_effect;
	}

	return nullptr;
}

void Particle_scene::register_factory(Particle_effect_factory* factory, const std::string& type)
{
	factories_[type] = factory;
}

}
