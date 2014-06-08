#pragma once

#include "Memory/Heap_cache.hpp"
#include <string>
#include <map>

namespace scene
{

class Particle_effect;
class Particle_effect_factory;

class Particle_scene
{

public:

	Particle_scene();

	void clear();

	Particle_effect* create_particle_effect(const std::string& type);

	void register_factory(Particle_effect_factory* factory, const std::string& type);

	void on_tick(float time_slice);

	void on_update(float frame_time, float speed);

	const Heap_cache<Particle_effect>& particle_effects() const;

private:

	std::map<std::string, Particle_effect_factory*> factories_;

	Heap_cache<Particle_effect> particle_effects_;
};

}
