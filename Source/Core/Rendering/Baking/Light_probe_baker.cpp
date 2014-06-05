#include "Light_probe_baker.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Environment_map_renderer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Texture_transfer.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Storage/Texture_storage_save.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "Rendering/Scene_rendering/Main_scene_renderer.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Texture_provider.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Light/Light_probe.hpp"
#include "Resources/Resource_manager.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"
#include "Math/Hammersley.hpp"
#include "String/String.hpp"
#include "File/File.hpp"
#include "Logging/Logging.hpp"

#include <iostream>

namespace rendering
{

namespace baking
{

// positive x, right
// negative x, left
// positive y, top
// negative y, bottom
// positive z, front
// negative z, back

// kind of inverted because of OpenGL texture coordinates convention?

const float3 Light_probe_baker::s_dirs[] =
{
    float3(1.f, 0.f, 0.f),  float3(-1.f, 0.f, 0.f), float3(0.f, 1.f, 0.f),
    float3(0.f, -1.f, 0.f), float3(0.f, 0.f, 1.f),  float3(0.f, 0.f, -1.f)
};

const float3 Light_probe_baker::s_ups[] =
{
    float3(0.f, -1.f, 0.f), float3(0.f, -1.f, 0.f), float3(0.f, 0.f, 1.f),
    float3(0.f, 0.f, -1.f), float3(0.f, -1.f, 0.f), float3(0.f, -1.f, 0.f)
};

Light_probe_baker::Light_probe_baker(Rendering_tool& rendering_tool) : rendering_tool_(rendering_tool), probe_texture_adapter_(nullptr)
{}

Light_probe_baker::~Light_probe_baker()
{
	delete [] roughness_;

	delete probe_texture_adapter_;
}

bool Light_probe_baker::init(Resource_manager& resource_manager, const uint2& dimensions)
{
	if (!scene::Light_probe::integrated_brdf())
	{
		scene::Light_probe::set_intergrated_brdf(create_integrated_brdf(1024, resource_manager));
	}

	dimensions_ = dimensions;

	Texture_description description;
	description.type = Texture_description::Type::Texture_cube;
	description.format = Data_format::R16G16B16A16_Float;
	description.dimensions = uint3(dimensions_, 1);
	description.num_layers = 6;
	description.num_mip_levels = Texture_description::calculate_num_mip_levels(description.dimensions) - 1;
	probe_texture_adapter_ = new Generic_texture_data_adapter(description);

	num_roughness_levels_ = description.num_mip_levels;
	roughness_ = new float[num_roughness_levels_];

	roughness_[0] = 2.f / 255.f;

	for (uint32_t r = 1; r < num_roughness_levels_; ++r)
	{
		roughness_[r] = float(r) / float(num_roughness_levels_ - 1);
	}

	effect_ = resource_manager.load<Effect>("Effects/Baking/Filter_environment_map.effect");
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), effect_->technique(0)->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_view_.init(effect_, "Change_per_view"))
	{
		return false;
	}

	if (!change_per_roughness_.init(effect_, "Change_per_roughness"))
	{
		return false;
	}

	return create_render_states();
}

bool Light_probe_baker::load_cached_data(scene::Scene& scene, Resource_manager& resource_manager) const
{
	auto& file_system = resource_manager.virtual_file_system();

	uint64_t scene_last_modified = file_system.file_last_modified(scene.name());

	std::string cache_load_name_template = get_cache_load_name_template(scene.name());

	if (scene.surrounding_light_probe())
	{
		std::string cache_load_name = cache_load_name_template + "_surrounding_light_probe.sui";

		uint64_t cache_last_modified = file_system.file_last_modified(cache_load_name);

	//	if (scene_last_modified > cache_last_modified)
		{
			return false;
		}

		Handle<Shader_resource_view> environment_map = resource_manager.load<Shader_resource_view>(cache_load_name);

		if (environment_map)
		{
			scene.surrounding_light_probe()->set_texture(environment_map);
		}
		else
		{
			return false;
		}
	}

	for (auto p : scene.light_probes())
	{
		std::string cache_load_name = cache_load_name_template + "_light_probe_0.sui";

		uint64_t cache_last_modified = file_system.file_last_modified(cache_load_name);

	//	if (scene_last_modified > cache_last_modified)
		{
			return false;
		}

		Handle<Shader_resource_view> environment_map = resource_manager.load<Shader_resource_view>(cache_load_name);

		if (environment_map)
		{
			p->set_texture(environment_map);
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Light_probe_baker::allocate_targets()
{
	auto& device = rendering_tool_.device();

	framebuffer_ = device.create_framebuffer();

	if (!framebuffer_)
	{
		return false;
	}

	Texture_description texture_description;
	texture_description.type = rendering::Texture_description::Type::Texture_2D;
	texture_description.format = rendering::Data_format::R16G16B16A16_Float;
	texture_description.dimensions = uint3(dimensions_, 0);
	texture_description.num_mip_levels = 1;

	filter_target_ = device.create_render_target_shader_resource_view(texture_description);

	if (!filter_target_)
	{
		return false;
	}

	framebuffer_->set_render_targets(filter_target_->render_target_view());

	if (framebuffer_->is_valid())
	{
		return false;
	}

	return true;
}

void Light_probe_baker::free_targets()
{
	filter_target_ = nullptr;
	framebuffer_ = nullptr;
}

void Light_probe_baker::bake(scene::Scene& scene, Environment_map_renderer& environment_map_renderer, Resource_manager& resource_manager, uint32_t pass)
{
	environment_map_renderer.configure_for_single_cube_map(dimensions_);

	Handle<Shader_resource_view>& color_shader_resource = environment_map_renderer.color_target()->shader_resource_view();

	Rendering_context::Rendering_options options;

	options.set(Rendering_context::Options::Render_actors, false);
	options.set(Rendering_context::Options::Render_particles, false);
	options.set(Rendering_context::Options::Render_surrounding, true);
	options.set(Rendering_context::Options::Face_culling, true);

	std::string cache_save_name_template = get_cache_save_name_template(scene.name(), resource_manager);

	if (scene.surrounding_light_probe() && 0 == pass)
	{
		options.set(Rendering_context::Options::Render_static_geometry, false);
		options.set(Rendering_context::Options::Render_analytical_lighting, false);
		options.set(Rendering_context::Options::Render_image_based_lighting, false);
		options.set(Rendering_context::Options::Render_emissive_lighting, false);

		environment_map_renderer.render(scene, float3::identity, options);

		std::string cache_save_name = cache_save_name_template + "_surrounding_light_probe.sui";

		scene.surrounding_light_probe()->set_texture(filter_environment_map(color_shader_resource, true, cache_save_name));
	}

	options.set(Rendering_context::Options::Render_static_geometry, true);
	options.set(Rendering_context::Options::Render_analytical_lighting, true);
	options.set(Rendering_context::Options::Render_image_based_lighting, pass > 0);
	options.set(Rendering_context::Options::Render_emissive_lighting, true);

	for (auto p : scene.light_probes())
	{
		environment_map_renderer.render(scene, p->world_position(), options);

		std::string cache_save_name = cache_save_name_template + "_light_probe_0.sui";

		p->set_texture(filter_environment_map(color_shader_resource, pass != 0, cache_save_name));
	}
}

Handle<Shader_resource_view> Light_probe_baker::filter_environment_map(const Handle<Shader_resource_view>& environment_map, bool cache, const std::string& cache_name)
{
	Rendering_device& device = rendering_tool_.device();

	Texture_description texture_description;
	texture_description.type = rendering::Texture_description::Type::Texture_2D;
	texture_description.format = rendering::Data_format::R16G16B16A16_Float;
	texture_description.dimensions = uint3(dimensions_, 1);
	texture_description.num_mip_levels = 1;

	Handle<Texture_transfer> texture_transfer = device.create_texture_transfer(texture_description);

	device.set_framebuffer(framebuffer_);

	device.set_rasterizer_state(rasterizer_state_);
	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_input_layout(input_layout_);

	effect_->use(device);

	device.set_shader_resources(1, &environment_map);

	scene::Camera camera;
	float3 position(0.f, 0.f, 0.f);
	float4x4 view;

	camera.set_projection(math::to_radians(90.f), 1.f, 0.1f, 100.f);

	uint32_t bytes_per_pixel = Data_format::num_bytes_per_block(texture_description.format);

	Viewport viewport;

	for (uint32_t i = 0; i < 6; ++i)
	{
		set_look_at_negative_x(view, position, position + s_dirs[i], s_ups[i]);
		camera.set_view(view);
		camera.update_frustum();

		const float3* rays = camera.view_rays_ws();
		auto& changer_per_view_data = change_per_view_.data();
		changer_per_view_data.ray0 = rays[0];
		changer_per_view_data.ray1 = rays[1];
		changer_per_view_data.ray2 = rays[2];
		change_per_view_.update(device);

		for (uint32_t r = 0; r < num_roughness_levels_; ++r)
		{
			Texture_description::Data data;
			probe_texture_adapter_->get_image(data, r, i);

			viewport.size = float2(data.dimensions.xy);
			device.set_viewports(1, &viewport);

			change_per_roughness_.data().roughness = roughness_[r];
			change_per_roughness_.update(device);

			effect_->technique(0)->use();

            rendering_tool_.render_fullscreen_effect();

			device.copy(texture_transfer, filter_target_->render_target_view());

			unsigned char* buffer;
			device.map(&buffer, texture_transfer);

			copy_region(buffer, texture_description.dimensions.xy, bytes_per_pixel, data.buffer, data.dimensions.xy);

			device.unmap(texture_transfer);
		}
	}

	if (cache)
	{
		rendering::texture_storage::save(cache_name, *probe_texture_adapter_, file::File_type::SUI);
	}

	Handle<Texture> texture = device.create_texture_cube(*probe_texture_adapter_);

	if (!texture)
	{
		return nullptr;
	}

	return device.create_shader_resource_view(texture, "");
}

bool Light_probe_baker::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_ = rendering_tool_.render_state_cache().rasterizer_state(rasterizer_description);
	if (!rasterizer_state_)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.stencil_enable = false;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	ds_state_ = rendering_tool_.render_state_cache().depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	blend_state_ = rendering_tool_.render_state_cache().blend_state(blend_description);
	if (!blend_state_)
	{
		return false;
	}

	return true;
}

float3 importance_sample_GGX(float2 xi, float roughness, float3 n)
{
	float a = roughness * roughness;

	float phi = 2.f * math::pi * xi.x;

	float cos_theta = sqrt((1.f - xi.y) / (1.f + (a * a - 1.f) * xi.y ));

	float sin_theta = sqrt(1.f - cos_theta * cos_theta );

	float3 h(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);

	float3 up = abs(n.z) < 0.999f ? float3(0.f, 0, 1.f) : float3(1.f, 0.f, 0.f);

	float3 tangent_x = normalize(cross(up, n));

	float3 tangent_y = cross(n, tangent_x);

	// Tangent to world space
	return h.x * tangent_x + h.y * tangent_y + h.z * n;
}

float g1(float n_dot_v, float k)
{
	return n_dot_v / (n_dot_v * (1.f - k) + k);
}

float g_smith(float roughness, float n_dot_l, float n_dot_v)
{
	float r1 = roughness + 1.f;
	float k = (r1 * r1) / 8.f;

	return g1(n_dot_l, k) * g1(n_dot_v, k);
}

float2 integrate_brdf(float roughness, float n_dot_v, uint32_t num_samples)
{
	const float3 n(0.f, 0.f, 1.f);

	n_dot_v = std::max(n_dot_v, 0.00001f);

	float3 v;
	v.x = sqrt(1.f - n_dot_v * n_dot_v); // sin
	v.y = 0.f;
	v.z = n_dot_v; // cos

	float a = 0.f;
	float b = 0.f;

	for (uint32_t i = 0; i < num_samples; ++i)
	{
		float2 xi = math::hammersley( i, num_samples);
		float3 h = importance_sample_GGX(xi, roughness, n);
		float3 l = 2.f * dot(v, h) * h - v;
		float n_dot_l = math::saturate(l.z);
		float n_dot_h = math::saturate(h.z);
		float v_dot_h = math::saturate(dot(v, h));

		if (n_dot_l > 0.f)
		{
			float g = g_smith(roughness, n_dot_v, n_dot_l);
			float g_vis = g * v_dot_h / (n_dot_h * n_dot_v);
			float fc = pow(1.f - v_dot_h, 5.f);
			a += (1.f - fc) * g_vis;
			b += fc * g_vis;
		}
	}

	return float2(a, b) / float(num_samples);
}

Handle<Shader_resource_view> Light_probe_baker::create_integrated_brdf(uint32_t num_samples, Resource_manager& /*resource_manager*/) const
{
//	Flags flags;
//	flags.set(rendering::Texture_provider::Flags::Treat_as_linear, true);

//	Handle<Shader_resource_view> lut = resource_manager.load<Shader_resource_view>("Textures/LUT/integrated_brdf_" + string::to_string(num_samples) + ".dds", flags);

//	if (lut)
//	{
//		return lut;
//	}

	const float norm = float(0xffff);
	typedef Vector2<unsigned short> ushort2;

	Texture_description description;
	description.type = Texture_description::Type::Texture_2D;
	description.format = Data_format::R16G16_UNorm;
	description.dimensions = uint3(32, 32, 0);

	Texture_description::Data data;
	data.dimensions = description.dimensions;
	data.num_bytes = description.dimensions.x * description.dimensions.y * sizeof(ushort2);
	data.buffer = new unsigned char[data.num_bytes];

	ushort2* integrated_brdf =  reinterpret_cast<ushort2*>(data.buffer);

	for (uint32_t y = 0; y < description.dimensions.y; ++y)
	{
		float v = y / float(description.dimensions.y - 1);

		for (uint32_t x = 0; x < description.dimensions.x; ++x)
		{
			float roughness = x / float(description.dimensions.x - 1);

			float2 brdf = norm * integrate_brdf(roughness, v, num_samples);

			integrated_brdf[y * description.dimensions.x + x] = ushort2((unsigned short)(brdf.x), (unsigned short)(brdf.y));
		}
	}

	Generic_texture_data_adapter texture_data_adapter(description, &data);

//	rendering::texture_storage::save("../../Data/Textures/LUT/integrated_brdf_" + string::to_string(num_samples) + ".dds", texture_data_adapter, file::File_type::DDS);

	auto& device = rendering_tool_.device();

	Handle<Texture> texture = device.create_texture_2D(texture_data_adapter);

	delete [] data.buffer;

	if (!texture)
	{
		return nullptr;
	}

	return device.create_shader_resource_view(texture, "");
}

void Light_probe_baker::copy_region(unsigned char* source, const uint2& source_dimensions, uint32_t bytes_per_pixel, unsigned char* destination, const uint2& destination_dimensions)
{
	if (destination_dimensions == source_dimensions)
	{
		uint32_t num_bytes = destination_dimensions.x * destination_dimensions.y * bytes_per_pixel;

		std::copy(source, source + num_bytes, destination);
	}
	else if (destination_dimensions.x <= source_dimensions.x && destination_dimensions.y <= source_dimensions.y)
	{
		uint32_t source_pitch = source_dimensions.x * bytes_per_pixel;
		uint32_t destination_pitch = destination_dimensions.x * bytes_per_pixel;

		for (uint32_t y = 0; y < destination_dimensions.y; ++y)
		{
			std::copy(source, source + destination_pitch, destination);

			source += source_pitch;
			destination += destination_pitch;
		}
	}
}

}

}
