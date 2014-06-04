#pragma once

#include <vector>
#include <cstdint>

template<typename T> struct Vector3;
typedef Vector3<float> float3;

namespace scene
{

class Particle_scene;
class Particle_system;

}

namespace rendering
{

struct Render_system
{
	Render_system(const scene::Particle_system* system, float distance);

	bool operator<(const Render_system& surface) const;

	uint64_t                      signature;
	const scene::Particle_system* system;
};

class Particle_collector
{

public:

	void collect(const scene::Particle_scene& scene, const float3& eye_position);

	const std::vector<Render_system>& systems() const;

private:

	std::vector<Render_system> systems_;

};

}
