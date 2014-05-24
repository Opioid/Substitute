#include "Environment_map_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Texture_transfer.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Storage/Texture_storage_save.hpp"
#include "Rendering/Scene_rendering/Main_scene_renderer.hpp"
#include "Scene/Scene.hpp"

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

const float3 Environment_map_renderer::s_directions[] =
{
	float3(1.f,  0.f, 0.f), float3(-1.f, 0.f, 0.f), float3(0.f, 1.f,  0.f),
	float3(0.f, -1.f, 0.f), float3( 0.f, 0.f, 1.f), float3(0.f, 0.f, -1.f)
};

const float3 Environment_map_renderer::s_ups[] =
{
    float3(0.f, 1.f, 0.f), float3(0.f, 1.f, 0.f), float3(0.f, 0.f, -1.f),
	float3(0.f, 0.f, 1.f), float3(0.f, 1.f, 0.f), float3(0.f, 1.f,  0.f)
};

Environment_map_renderer::Environment_map_renderer(Rendering_tool& rendering_tool, Main_scene_renderer& scene_renderer) :
	rendering_tool_(rendering_tool), scene_renderer_(scene_renderer)
{
	context_.set_camera(camera_);
}

bool Environment_map_renderer::allocate_targets(const uint2& dimensions)
{
	auto& device = rendering_tool_.device();

	target_dimensions_ = dimensions;

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_cube;
	texture_description.format = Data_format::R16G16B16A16_Float;
	texture_description.dimensions = uint3(target_dimensions_, 0);

	color_target_ = device.create_cube_render_target_shader_resource_view(texture_description);

	if (!color_target_)
	{
		return false;
	}

	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.format = Data_format::D24_UNorm_S8_UInt;

	depth_stencil_ = device.create_depth_stencil_shader_resource_view(texture_description);

	if (!depth_stencil_)
	{
		return false;
	}

	framebuffer_ = device.create_framebuffer();

	if (!framebuffer_)
	{
		return false;
	}

	context_.set_framebuffer(framebuffer_);

	return scene_renderer_.resize_targets(dimensions, depth_stencil_);
}

void Environment_map_renderer::free_targets()
{
	framebuffer_ = nullptr;
	depth_stencil_ = nullptr;
	color_target_ = nullptr;
}

void Environment_map_renderer::configure_for_single_cube_map(const uint2& face_dimensions)
{
	camera_.set_projection(math::to_radians(90.f), 1.f, 0.1f, 100.f);

	face_dimensions_ = face_dimensions;
}

void Environment_map_renderer::configure_for_flattened_cube_map_batch(const uint2& face_dimensions)
{
	camera_.set_projection(math::to_radians(90.f), 1.f, 0.02f, 50.f, true);

	face_dimensions_ = face_dimensions;
}

const uint2& Environment_map_renderer::target_dimensions() const
{
	return target_dimensions_;
}

Handle<Framebuffer>& Environment_map_renderer::framebuffer()
{
	return framebuffer_;
}

Handle<Cube_render_target_shader_resource_view>& Environment_map_renderer::color_target()
{
	return color_target_;
}

void Environment_map_renderer::render(const scene::Scene& scene, const float3& position, const Rendering_context::Rendering_options& options)
{
	Rendering_device& device = rendering_tool_.device();

	context_.set_options(options);

	Viewport viewport(float2(0.f, 0.f), float2(face_dimensions_));
	context_.set_viewport(viewport);

	for (uint32_t i = 0; i < 6; ++i)
	{
		framebuffer_->set_render_targets(color_target_->render_tarview(i), depth_stencil_->depth_stencil_view());

		Color4 clear_color(0.f, 0.f, 0.f, 1.f);
		device.clear_color(framebuffer_, 1, &clear_color);
		device.clear_depth_stencil(framebuffer_, 1.f, 0);

		float4x4 view;
		// negate direction because of OpenGL texture coordinates convention?
		set_look_at(view, position, position + (-s_directions[i]), s_ups[i]);
		camera_.set_view(view);
		camera_.update_frustum();

		scene_renderer_.render(scene, context_);
	}

	device.set_framebuffer(nullptr);
}

void Environment_map_renderer::start_batch(const Rendering_context::Rendering_options& options)
{
	Rendering_device& device = rendering_tool_.device();

	context_.set_options(options);

	framebuffer_->set_render_targets(color_target_->render_tarview(0), depth_stencil_->depth_stencil_view());

	Color4 clear_color(0.f, 0.f, 0.f, 1.f);
	device.clear_color(framebuffer_, 1, &clear_color);
	device.clear_depth_stencil(framebuffer_, 1.f, 0);
}

void Environment_map_renderer::render(const scene::Scene& scene, const float3& position, uint32_t batch_index)
{
	Rendering_device& device = rendering_tool_.device();

	for (uint32_t i = 0; i < 6; ++i)
	{
		Viewport viewport(float2(float(i * face_dimensions_.x), /*float(target_dimensions_.y - (batch_index + 1) * face_dimensions_.y)*/ float(batch_index * face_dimensions_.y)), float2(face_dimensions_));
		context_.set_viewport(viewport);

		float4x4 view;
		set_look_at(view, position, position + s_directions[i], s_ups[i]);
		camera_.set_view(view);
		camera_.update_frustum();

		scene_renderer_.render(scene, context_);
	}

	device.set_framebuffer(nullptr);
}

Main_scene_renderer& Environment_map_renderer::scene_renderer() const
{
	return scene_renderer_;
}

}

}
