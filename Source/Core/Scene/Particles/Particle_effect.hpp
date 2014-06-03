#pragma once

#include "Scene/Entity.hpp"
#include "Resources/Handle.hpp"

namespace scene
{

class Particle_system;
class Material;

class Particle_effect : public Entity
{

public:

	Particle_effect();
	~Particle_effect();

	void reserve(uint32_t num_systems);

	uint32_t num_systems() const;
	const Particle_system* system(uint32_t index) const;

	void set_system(uint32_t index, Particle_system* system);

	void clear();

	void set_materials(uint32_t num_materials, const Handle<Material> materials[]);

	void on_tick(float time_slice);

private:

	uint32_t num_systems_;
	uint32_t capacity_;

	Particle_system** systems_;
};

}
