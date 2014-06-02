#pragma once

#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"

namespace rendering
{

class Vertex_layout_description;

}

namespace scene
{

class Material;

class Particle_system
{

public:

	struct Vertex
	{
		float4 position;
		float  size;

		static const rendering::Vertex_layout_description* vertex_layout_description();
	};

	Particle_system(uint32_t num_particles);
	virtual ~Particle_system();

	void set_material(const Handle<Material>& material);

	uint32_t num_particles() const;

	virtual void on_tick(float time_slice) = 0;

private:

	Handle<Material> material_;

	uint32_t num_particles_;
};

}
