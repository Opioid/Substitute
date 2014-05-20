#pragma once

#include "Surrounding_renderer.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Rendering/Render_state.hpp"
#include "Math/Vector.hpp"

namespace rendering
{

class Effect;

class Monochrome_surrounding_renderer : public Surrounding_renderer
{

public:

	Monochrome_surrounding_renderer(Rendering_tool& rendering_tool);
	virtual ~Monochrome_surrounding_renderer();

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const scene::Scene& scene, const Rendering_context& context);

	virtual void clear(const Color3& color, const Rendering_context& context);

private:

	bool create_render_states();

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	struct Clear_buffer
	{
		Color3 surrounding_color;
	};

	Constant_buffer_updater<Clear_buffer> color_buffer_;

	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state> blend_state_;
};

}


