#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Vector.hpp"

class Resource_manager;

namespace scene
{

class Particle_scene;
class Material;

}

namespace rendering
{

class Constant_buffer_cache;

class Particle_renderer : public Rendering_object
{

public:

	Particle_renderer(Rendering_tool& rendering_tool);

	bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void render(const scene::Particle_scene& scene, float interpolation_delta, const Rendering_context& context);

private:

	void prepare_material(const scene::Material* material);

	bool create_buffers();
	bool create_render_states();

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	const scene::Material* previous_material_;

	struct
	{
		Effect_technique* particle;
	}
	techniques_;

	uint32_t num_vertices_;

	Handle<Vertex_buffer> vertex_buffers_[2];

	uint32_t strides_[2];

	struct Change_per_frame
	{
		float2 time;
	};

	Constant_buffer_updater<Change_per_frame> change_per_frame_;

	Handle<Rasterizer_state> rasterizer_state_;

	Handle<Depth_stencil_state> ds_state_;

	Handle<Blend_state> blend_state_;
	Handle<Blend_state> alpha_blend_state_;
};

}
