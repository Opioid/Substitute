#include "Sampler_state_cache.hpp"
#include "Rendering/Rendering_device.hpp"

namespace rendering
{

Sampler_state_cache::Sampler_state_cache(const Rendering_device& device) : Cache(device)
{}

void Sampler_state_cache::release()
{
	for (auto s : sampler_states_)
	{
		delete s;
	}

	sampler_states_.clear();
}

Handle<Sampler_state> Sampler_state_cache::get_sampler_state(const Sampler_state::Description& description)
{
	for (auto s : sampler_states_)
	{
		if (s->description == description)
		{
			return s->sampler_state;
		}
	}

	Handle<Sampler_state> sampler_state = device_.create_sampler_state(description);

	if (!sampler_state)
	{
		return nullptr;
	}

	Sampler_state_slot* slot = new Sampler_state_slot{ description, sampler_state };
	sampler_states_.push_back(slot);

	return sampler_state;
}

}
