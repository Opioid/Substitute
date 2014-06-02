#include "Particle_system.hpp"
#include "Scene/Material.hpp"

namespace scene
{

Particle_system::~Particle_system()
{}

void Particle_system::set_material(const Handle<Material>& material)
{
	material_ = material;
}

}
