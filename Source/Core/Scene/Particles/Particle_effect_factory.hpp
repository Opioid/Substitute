#pragma once

namespace scene
{

class Particle_effect;

class Particle_effect_factory
{

public:

	virtual bool init_particle_effect(Particle_effect& effect) = 0;
};

}
