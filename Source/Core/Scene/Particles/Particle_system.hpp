#pragma once

#include "Resources/Handle.hpp"

namespace scene
{

class Material;

class Particle_system
{

public:

	virtual ~Particle_system();

	void set_material(const Handle<Material>& material);

	virtual void on_tick(float time_slice) = 0;

private:

	Handle<Material> material_;
};

}
