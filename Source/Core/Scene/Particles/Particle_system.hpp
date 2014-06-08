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
		float  angle;
		float2 scale;
		float  index;
		float  alpha;
	};

	Particle_system(uint32_t num_particles);
	virtual ~Particle_system();

	const Particle_effect* parent() const;
	void set_parent(const Particle_effect* parent);

	const Handle<Material>& material() const;
	void set_material(const Handle<Material>& material);

	uint32_t num_particles() const;

	const Vertex* vertices() const;
	Vertex* vertices();

	virtual void on_tick(float time_slice);

	virtual void on_update(float frame_time, float speed);

protected:

	const Particle_effect* parent_;

	Handle<Material> material_;

private:

	uint32_t num_particles_;

	Vertex* vertices_;
};

}
