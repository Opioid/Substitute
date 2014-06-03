#pragma once

#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"

namespace scene
{

class Particle_effect;
class Material;

class Particle_system
{

public:

	struct Vertex
	{
		float3 position;
		float4 properties;
	};

	Particle_system(uint32_t num_particles);
	virtual ~Particle_system();

	const Handle<Material>& material() const;
	void set_material(const Handle<Material>& material);

	uint32_t num_particles() const;

	const Vertex* previous_vertices() const;

	const Vertex* current_vertices() const;
	Vertex* current_vertices();

	void on_tick(const Particle_effect& effect, float time_slice);

private:

	virtual void private_on_tick(const Particle_effect& effect, float time_slice) = 0;

	Handle<Material> material_;

	uint32_t num_particles_;

	Vertex* previous_vertices_;
	Vertex* current_vertices_;
};

}
