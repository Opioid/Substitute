#include "Particle_system.hpp"
#include "Scene/Material.hpp"
#include "Math/Vector.inl"
#include <algorithm>

namespace scene
{

Particle_system::Particle_system(uint32_t num_particles) : parent_(nullptr), num_particles_(num_particles)
{
	previous_vertices_ = new Vertex[num_particles_];
	current_vertices_  = new Vertex[num_particles_];
}

Particle_system::~Particle_system()
{
	delete [] current_vertices_;
	delete [] previous_vertices_;
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

const Particle_system::Vertex* Particle_system::previous_vertices() const
{
	return previous_vertices_;
}

const Particle_system::Vertex* Particle_system::current_vertices() const
{
	return current_vertices_;
}

Particle_system::Vertex* Particle_system::current_vertices()
{
	return current_vertices_;
}

void Particle_system::on_tick(float time_slice)
{
	std::copy(current_vertices_, current_vertices_ + num_particles_, previous_vertices_);

	private_on_tick(time_slice);
}

}
