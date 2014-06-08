#include "Particle_system.hpp"
#include "Scene/Material.hpp"
#include "Math/Vector.inl"

namespace scene
{

Particle_system::Particle_system(uint32_t num_particles) : parent_(nullptr), num_particles_(num_particles)
{
	vertices_ = new Vertex[num_particles_];
}

Particle_system::~Particle_system()
{
	delete [] vertices_;
}

const Particle_effect* Particle_system::parent() const
{
	return parent_;
}

void Particle_system::set_parent(const Particle_effect* parent)
{
	parent_ = parent;
}

const Handle<Material>& Particle_system::material() const
{
	return material_;
}

void Particle_system::set_material(const Handle<Material>& material)
{
	material_ = material;
}

uint32_t Particle_system::num_particles() const
{
	return num_particles_;
}

const Particle_system::Vertex* Particle_system::vertices() const
{
	return vertices_;
}

Particle_system::Vertex* Particle_system::vertices()
{
	return vertices_;
}

void Particle_system::on_tick(float /*time_slice*/)
{}

void Particle_system::on_update(float /*frame_time*/, float /*speed*/)
{}

}
