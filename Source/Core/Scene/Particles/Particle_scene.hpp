#pragma once

#include <string>
#include <map>

namespace scene
{

class Particle_effect;
class Particle_effect_factory;

class Particle_scene
{

public:

	Particle_effect* create_particle_effect(const std::string& type);

	void register_factory(Particle_effect_factory* factory, const std::string& type);

private:

	std::map<std::string, Particle_effect_factory*> factories_;
};

}
