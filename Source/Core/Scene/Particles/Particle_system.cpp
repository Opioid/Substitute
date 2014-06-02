#include "Particle_system.hpp"
#include "Scene/Material.hpp"
#include "Rendering/Vertex_layout_description.hpp"

namespace scene
{

const rendering::Vertex_layout_description* Particle_system::Vertex::vertex_layout_description()
{
	rendering::Vertex_layout_description::Element elements[] =
	{
		rendering::Vertex_layout_description::Element("Position", 0, rendering::Data_format::R32G32B32A32_Float)
	};

	static const rendering::Vertex_layout_description desc(1, elements);

	return &desc;
}

Particle_system::Particle_system(uint32_t num_particles) : num_particles_(num_particles)
{}

Particle_system::~Particle_system()
{}

void Particle_system::set_material(const Handle<Material>& material)
{
	material_ = material;
}

uint32_t Particle_system::num_particles() const
{
	return num_particles_;
}

}
