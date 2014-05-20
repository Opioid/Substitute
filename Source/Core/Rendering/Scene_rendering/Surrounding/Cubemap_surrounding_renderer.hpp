#pragma once

#include "Surrounding_renderer.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Vector.hpp"

namespace rendering
{

class Cubemap_surrounding_renderer : public Surrounding_renderer
{

public:

	Cubemap_surrounding_renderer(Rendering_tool& rendering_tool);
	virtual ~Cubemap_surrounding_renderer();

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const scene::Scene& scene, const Rendering_context& context);

	virtual void clear(const Color3& color, const Rendering_context& context);

private:

	bool create_render_states();

	Handle<Effect> effect_;

	struct Change_per_camera
	{
		float4 rays[3];
	};

	Constant_buffer_updater<Change_per_camera> change_per_camera_;

	struct Clear_buffer
	{
		Color3 clear_color;
	};

	Constant_buffer_updater<Clear_buffer> clear_buffer_;

	Handle<Input_layout> input_layout_;

	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state> blend_state_;
};

}


