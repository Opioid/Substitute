#pragma once

#include "Cache.hpp"
#include "Rendering/Sampler_state.hpp"
#include "Resources/Handle.hpp"
#include <list>

namespace rendering
{

class Sampler_state_cache : public Cache
{

public:

	Sampler_state_cache(const Rendering_device& device);

	virtual void release();

	Handle<Sampler_state> sampler_state(const Sampler_state::Description& description);

private:

	struct Sampler_state_slot
	{
		Sampler_state::Description description;
		Handle<Sampler_state> sampler_state;
	};

	std::list<Sampler_state_slot*> sampler_states_;
};

}


