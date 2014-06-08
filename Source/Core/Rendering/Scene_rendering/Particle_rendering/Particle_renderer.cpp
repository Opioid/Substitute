#include "Particle_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect_provider.hpp"
#include "Rendering/Constant_buffer_cache.hpp"
#include "Rendering/Resource_view.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Material.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Particles/Particle_scene.hpp"
#include "Scene/Particles/Particle_effect.hpp"
#include "Scene/Particles/Particle_system.hpp"
#include "Math/Vector.inl"

namespace rendering
{

Particle_renderer::Particle_renderer(Rendering_tool& rendering_tool) :
	Rendering_object(rendering_tool),
	previous_material_(nullptr),
	previous_technique_(nullptr),
	previous_blend_state_(0xFFFFFFFF),
	num_vertices_(1024)
{}

bool Particle_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache)
{
	Flags<Effect_provider::Options, uint32_t> flags;
	flags.set(Effect_provider::Options::Use_custom_constant_buffers, true);

	effect_ = resource_manager.load<Effect>("Effects/Particle_rendering/Particle_renderer.effect", flags.data());
	if (!effect_)
	{
		return false;
	}

	techniques_.color_map      = effect_->technique("Color_map");
	techniques_.color_map_soft = effect_->technique("Color_map_soft");

	techniques_.array_color_map      = effect_->technique("Array_color_map");
	techniques_.array_color_map_soft = effect_->technique("Array_color_map_soft");

	Vertex_layout_description::Element elements[] =
	{
		Vertex_layout_description::Element("Previous_position",   0, Data_format::R32G32B32A32_Float, 0),
		Vertex_layout_description::Element("Previous_properties", 0, Data_format::R32G32B32A32_Float, 0),
		Vertex_layout_description::Element("Current_position",    0, Data_format::R32G32B32A32_Float, 1),
		Vertex_layout_description::Element("Current_properties",  0, Data_format::R32G32B32A32_Float, 1)
	};

	static const Vertex_layout_description description(4, elements);

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(description, techniques_.color_map->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	color_texture_offset_		= effect_->sampler_offset("g_color_map");
	color_texture_array_offset_ = effect_->sampler_offset("g_color_map_array");

	auto& device = rendering_tool_.device();

	if (!change_per_frame_.init(effect_, "Change_per_frame"))
	{
		return false;
	}

	Constant_buffer_adapter* change_per_frame_adapter = change_per_frame_.adapter();
	Handle<Constant_buffer> change_per_frame_buffer = device.create_constant_buffer(change_per_frame_adapter->num_bytes());
	change_per_frame_adapter->set_constant_buffer(change_per_frame_buffer);

	constant_buffer_cache.set_constant_buffer("Change_per_frame", change_per_frame_buffer);

	Constant_buffer_adapter* change_per_camera_adapter = effect_->constant_buffer_adapter("Change_per_camera");
	if (!change_per_camera_adapter)
	{
		return false;
	}

	if (!constant_buffer_cache.connect(change_per_camera_adapter, "Change_per_camera"))
	{
		return false;
	}

	effect_->create_default_constant_buffers(device);

	if (!create_buffers())
	{
		return false;
	}

	return create_render_states();
}

void Particle_renderer::set_depth_texture(const Handle<Shader_resource_view>& depth)
{
	depth_texture_ = depth;
}

void Particle_renderer::render(const scene::Particle_scene& scene, float interpolation_delta, const Rendering_context& context)
{
	particle_collector_.collect(scene, context.camera().world_position());

	Rendering_device& device = rendering_tool_.device();

	device.set_viewports(1, &context.viewport());
	device.set_framebuffer(context.framebuffer());

	device.set_rasterizer_state(rasterizer_state_);
	device.set_depth_stencil_state(ds_state_);

	device.set_primitive_topology(Primitive_topology::Point_list);

	device.set_input_layout(input_layout_);

	device.set_vertex_buffers(2, vertex_buffers_, strides_);

	device.set_shader_resources(1, &depth_texture_);

	effect_->use(device);

	auto& change_per_frame_data = change_per_frame_.data();
	change_per_frame_data.time.x = interpolation_delta;
	change_per_frame_.update(device);

	previous_material_    = nullptr;
	previous_technique_   = nullptr;
	previous_blend_state_ = 0xFFFFFFFF;

	for (auto s : particle_collector_.systems())
	{
		const auto system = s.system;

		const scene::Material* material = system->material();

		prepare_material(material);

		device.update_buffer(*vertex_buffers_[0], 0, sizeof(scene::Particle_system::Vertex) * system->num_particles(), system->previous_vertices(), true);
		device.update_buffer(*vertex_buffers_[1], 0, sizeof(scene::Particle_system::Vertex) * system->num_particles(), system->current_vertices(), true);

		device.draw(system->num_particles());
	}
}

void Particle_renderer::prepare_material(const scene::Material* material)
{
	if (previous_material_ == material)
	{
		return;
	}

	auto& device = rendering_tool_.device();

	uint32_t blend_state = static_cast<uint32_t>(material->blending());

	if (previous_blend_state_ != blend_state)
	{
		device.set_blend_state(blend_states_[blend_state]);

		previous_blend_state_ = blend_state;
	}

	const Effect_technique* technique;

	if (material->is_array())
	{
		device.set_shader_resources(1, material->textures(), color_texture_array_offset_);

		technique = material->is_soft_particle() ? techniques_.array_color_map_soft : techniques_.array_color_map;
	}
	else
	{
		device.set_shader_resources(1, material->textures(), color_texture_offset_);

		technique = material->is_soft_particle() ? techniques_.color_map_soft : techniques_.color_map;
	}

	if (previous_technique_ != technique)
	{
		technique->use();
		previous_technique_ = technique;
	}

	previous_material_ = material;
}

bool Particle_renderer::create_buffers()
{
	strides_[0] = static_cast<uint32_t>(sizeof(scene::Particle_system::Vertex));
	strides_[1] = strides_[0];

	uint32_t stream_size = strides_[0] * num_vertices_;

	vertex_buffers_[0] = rendering_tool_.device().create_vertex_buffer(stream_size);
	if (!vertex_buffers_[0])
	{
		return false;
	}

	vertex_buffers_[1] = rendering_tool_.device().create_vertex_buffer(stream_size);
	if (!vertex_buffers_[1])
	{
		return false;
	}

	return true;
}

bool Particle_renderer::create_render_states()
{
	auto& cache = rendering_tool_.render_state_cache();

	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_ = cache.rasterizer_state(rasterizer_description);
	if (!rasterizer_state_)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = true;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;
	ds_description.stencil_enable = false;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	ds_state_ = cache.depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable            = false;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::All;

	blend_states_[0] = cache.blend_state(blend_description);
	if (!blend_states_[0])
	{
		return false;
	}

	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable            = true;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::Source_alpha;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::Inverse_source_alpha;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::All;

	blend_states_[1] = cache.blend_state(blend_description);
	if (!blend_states_[1])
	{
		return false;
	}

	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable            = true;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::All;

	blend_states_[2] = cache.blend_state(blend_description);
	if (!blend_states_[2])
	{
		return false;
	}

	return true;
}

}
