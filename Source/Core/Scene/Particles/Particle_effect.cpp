#include "Particle_effect.hpp"
#include "Particle_system.hpp"
#include "Scene/Material.hpp"

namespace scene
{

Particle_effect::Particle_effect() : num_systems_(0), capacity_(0), systems_(nullptr)
{}

Particle_effect::~Particle_effect()
{
	clear();

	delete [] systems_;
}

void Particle_effect::reserve(uint32_t num_systems)
{
	clear();

	if (capacity_ < num_systems)
	{
		delete [] systems_;

		systems_ = new Particle_system*[num_systems];

		capacity_ = num_systems;
	}

	num_systems_ = num_systems;
}

void Particle_effect::set_system(uint32_t index, Particle_system* system)
{
	systems_[index] = system;
}

void Particle_effect::clear()
{
	for (uint32_t i = 0; i < capacity_; ++i)
	{
		delete systems_[i];
		systems_[i] = nullptr;
	}

	num_systems_ = 0;
}

void Particle_effect::set_materials(uint32_t num_materials, const Handle<Material> materials[])
{
	uint32_t count = std::min(num_materials, num_systems_);
	for (uint32_t i = 0; i < count; ++i)
	{
		systems_[i]->set_material(materials[i]);
	}
}

void Particle_effect::on_tick(float time_slice)
{
	for (uint32_t i = 0; i < num_systems_; ++i)
	{
		systems_[i]->on_tick(time_slice);
	}
}

}
