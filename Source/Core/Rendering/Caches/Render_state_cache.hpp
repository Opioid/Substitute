#pragma once

#include "Cache.hpp"
#include "Rendering/Render_state.hpp"
#include "Resources/Handle.hpp"
#include <list>

namespace rendering
{

class Render_state_cache : public Cache
{

public:

	Render_state_cache(const Rendering_device& device);

	virtual void release();

	Handle<Rasterizer_state>    rasterizer_state(const Rasterizer_state::Description& description);
	Handle<Depth_stencil_state> depth_stencil_state(const Depth_stencil_state::Description& description);
	Handle<Blend_state>         blend_state(const Blend_state::Description& description);

private:

	struct Rasterizer_state_slot
	{
		Rasterizer_state::Description description;
		Handle<Rasterizer_state> state;
	};

	std::list<Rasterizer_state_slot> m_rasterizer_states;

	struct Depth_stencil_state_slot
	{
		Depth_stencil_state::Description description;
		Handle<Depth_stencil_state> state;
	};

	std::list<Depth_stencil_state_slot> m_depth_stencil_states;

	struct Blend_state_slot
	{
		Blend_state::Description description;
		Handle<Blend_state> state;
	};

	std::list<Blend_state_slot> blend_state_s;
};

}

