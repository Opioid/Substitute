#include "Render_state_cache.hpp"
#include "Rendering/Rendering_device.hpp"

namespace rendering
{

Render_state_cache::Render_state_cache(const Rendering_device& device) : Cache(device)
{}

void Render_state_cache::release()
{
	m_rasterizer_states.clear();
}

Handle<Rasterizer_state> Render_state_cache::rasterizer_state(const Rasterizer_state::Description& description)
{
	for (const auto& s : m_rasterizer_states)
	{
		if (s.description == description)
		{
			return s.state;
		}
	}

	Handle<Rasterizer_state> state = device_.create_rasterizer_state(description);

	Rasterizer_state_slot slot{ description, state };

	m_rasterizer_states.push_back(slot);

	return state;
}

Handle<Depth_stencil_state> Render_state_cache::depth_stencil_state(const Depth_stencil_state::Description& description)
{
	for (const auto& s : m_depth_stencil_states)
	{
		if (s.description == description)
		{
			return s.state;
		}
	}

	Handle<Depth_stencil_state> state = device_.create_depth_stencil_state(description);

	Depth_stencil_state_slot slot{ description, state };

	m_depth_stencil_states.push_back(slot);

	return state;
}

Handle<Blend_state> Render_state_cache::blend_state(const Blend_state::Description& description)
{
	for (const auto& s : blend_state_s)
	{
		if (s.description == description)
		{
			return s.state;
		}
	}

	Handle<Blend_state> state = device_.create_blend_state(description);

	Blend_state_slot slot{ description, state };

	blend_state_s.push_back(slot);

	return state;
}

}
