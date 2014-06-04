#include "Particle_collector.hpp"
#include "Scene/Material.hpp"
#include "Scene/Particles/Particle_scene.hpp"
#include "Scene/Particles/Particle_effect.hpp"
#include "Scene/Particles/Particle_system.hpp"

namespace rendering
{

Render_system::Render_system(const scene::Particle_system* system, float distance) : system(system)
{
	static const uint32_t width_16 = 0x0000FFFF;
	static const uint32_t width_20 = 0x000FFFFF;

	const scene::Material* material = system->material();

	uint32_t material_part = width_20 & (material->property_mask() << 16 | (width_16 & material->manageable_id()));

	signature = uint64_t(distance * 1000.f) << 32 | material_part;
}

bool Render_system::operator<(const Render_system& system) const
{
	return signature > system.signature;
}

void Particle_collector::collect(const scene::Particle_scene& scene, const float3& eye_position)
{
	systems_.clear();

	for (auto particle_effect : scene.particle_effects())
	{
		if (!particle_effect->is_visible())
		{
			continue;
		}

		float dist = squared_distance(particle_effect->world_position(), eye_position);

		uint32_t count = particle_effect->num_systems();
		for (uint32_t i = 0; i < count; ++i)
		{
			systems_.push_back(Render_system(particle_effect->system(i), dist));
		}
	}

	std::sort(systems_.begin(), systems_.end());
}

const std::vector<Render_system>& Particle_collector::systems() const
{
	return systems_;
}

}
