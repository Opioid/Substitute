#include "Renderer.hpp"
#include "Rendering_tool.hpp"
#include "Scene_rendering/Surrounding/Cubemap_surrounding_renderer.hpp"
#include "Scene_rendering/Deferred_scene_renderer.hpp"
#include "Scene_rendering/Test_scene_renderer0.hpp"
#include "Scene_rendering/Shadowing/Directional_shadow_renderer_EVSM.hpp"
#include "Scene_rendering/Shadowing/Directional_shadow_renderer_EVSM_MS.hpp"
#include "Scene_rendering/Shadowing/Spot_shadow_renderer_EVSM_MS.hpp"
#include "Post_processing/Hdr/Truncated_hdr.hpp"
#include "Post_processing/Hdr/Static_hdr.hpp"
#include "Post_processing/Anti_aliasing/Fxaa.hpp"
#include "Baking/Light_baker.hpp"
#include "Scene_rendering/Help_visualization/Bounding_renderer.hpp"
#include "Scene_rendering/Help_visualization/Editor_renderer.hpp"
#include "Scene_rendering/Help_visualization/Help_visualization_light_renderer.hpp"
#include "Rendering/Vertex_layout_description.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Storage/Texture_storage.hpp"
#include "Scene/Model.hpp"
#include "Application/Application.hpp"
#include "Resources/Resource_manager.hpp"
#include "Logging/Logging.hpp"
#include <iostream>

namespace rendering
{

Renderer::Renderer(Rendering_tool& rendering_tool) :
	rendering_tool_(rendering_tool), directional_shadow_renderer_(nullptr), spot_shadow_renderer_(nullptr),
	scene_renderer_(nullptr), post_processing_(rendering_tool),
	hdr_filter_(nullptr), fxaa_(nullptr),
	render_analytical_lighting_(true), render_image_based_lighting_(true),
	render_bounding_volumes_(false), bounding_renderer_(nullptr),
	editor_renderer_(nullptr),
	render_help_light_(false), help_light_renderer_(nullptr)
{}

bool Renderer::init(Application& app, Resource_manager& resource_manager, bool enable_fxaa)
{
	if (!texture_storage::init())
	{
		return false;
	}

	Rendering_device& device = rendering_tool_.get_device();

	logging::post(device.get_api_info());

	directional_shadow_renderer_ = new Directional_shadow_renderer_EVSM_MS(rendering_tool_, surface_collector_, uint2(1024, 1024));

	spot_shadow_renderer_ = new Spot_shadow_renderer_EVSM_MS(rendering_tool_, surface_collector_, uint2(1024, 1024));

	surrounding_renderer_ = new Cubemap_surrounding_renderer(rendering_tool_);
	if (!surrounding_renderer_->init(resource_manager))
	{
		logging::error("Surrounding renderer could not be initialized.");
		return false;
	}

	scene_renderer_ = new Deferred_scene_renderer(rendering_tool_, surface_collector_, *surrounding_renderer_, *directional_shadow_renderer_, *spot_shadow_renderer_);

//	scene_renderer_ = new Test_scene_renderer0(rendering_tool_, surface_collector_, *surrounding_renderer_);

	const uint2& virtual_size = rendering_tool_.get_virtual_size();

	Constant_buffer_cache constant_buffer_cache;

	if (!scene_renderer_->init(resource_manager, constant_buffer_cache) || !scene_renderer_->resize_targets(virtual_size, rendering_tool_.get_main_depth_stencil()))
	{
		logging::error("Main scene renderer could not be initialized.");
		return false;
	}

	Data_format::Value hdr_format = Data_format::R16G16B16A16_Float;

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.dimensions.xy = virtual_size;
	texture_description.format = hdr_format;

	hdr_target_ = device.create_render_target_shader_resource_view(texture_description);
	if (!hdr_target_)
	{
		return false;
	}

	framebuffer_ = device.create_framebuffer();
	if (!framebuffer_)
	{
		return false;
	}

	framebuffer_->set_render_targets(hdr_target_->get_render_target_view(), rendering_tool_.get_main_depth_stencil()->get_depth_stencil_view());

	if (!framebuffer_->is_valid())
	{
		return false;
	}

	const auto& scene  = app.get_scene();
	const auto& camera = scene.get_camera();

	hdr_context_.set_viewport(rendering_tool_.get_virtual_viewport());
	hdr_context_.set_framebuffer(framebuffer_);
	hdr_context_.set_camera(camera);

	presentation_context_.set_viewport(rendering_tool_.get_viewport());
	presentation_context_.set_framebuffer(rendering_tool_.get_default_framebuffer());
	presentation_context_.set_camera(camera);

	if (!post_processing_.init(resource_manager) || !post_processing_.resize_targets(virtual_size, Data_format::R8G8B8A8_UNorm_sRGB))
	{
		logging::error("Post processing could not be initialized.");
		return false;
	}

	hdr_filter_ = new Static_hdr(rendering_tool_, hdr_format);
	if (!hdr_filter_->init(resource_manager) || !hdr_filter_->resize_targets(virtual_size))
	{
		logging::error("Hdr filter could not be initialized.");
		return false;
	}

	post_processing_.set_hdr_effect(hdr_filter_);

	if (enable_fxaa)
	{
		fxaa_ = new Fxaa(rendering_tool_);
		if (!fxaa_->init(resource_manager))
		{
			return false;
		}

		post_processing_.push_back_effect(fxaa_);
	}

	light_baker_ = new baking::Light_baker(rendering_tool_, *scene_renderer_);
	if (!light_baker_->init(resource_manager, uint2(512, 512)))
	{
		return false;
	}

	bounding_renderer_ = new Bounding_renderer(rendering_tool_);
	if (!bounding_renderer_->init(resource_manager, constant_buffer_cache))
	{
		return false;
	}

	editor_renderer_ = new Editor_renderer(rendering_tool_);
	if (!editor_renderer_->init(resource_manager, constant_buffer_cache))
	{
		return false;
	}

	help_light_renderer_ = new Help_visualization_light_renderer(rendering_tool_);
	if (!help_light_renderer_->init(resource_manager, constant_buffer_cache))
	{
		logging::error("Help light renderer could not be initialized.");
		return false;
	}

	return true;
}

void Renderer::release()
{
	delete help_light_renderer_;

	delete bounding_renderer_;

	delete light_baker_;

	delete fxaa_;

	delete hdr_filter_;

	delete scene_renderer_;

	delete surrounding_renderer_;

	delete spot_shadow_renderer_;

	delete directional_shadow_renderer_;

	texture_storage::release();
}

baking::Light_baker* Renderer::get_light_baker()
{
	return light_baker_;
}

void Renderer::render(Application& app, float /*speed*/)
{
	Rendering_device& device = rendering_tool_.get_device();

	Color4 clear_color(0.f, 0.f, 0.f, 0.f);
	device.clear_color(framebuffer_, 1, &clear_color);
	device.clear_depth_stencil(framebuffer_, 1.f, 0);

	auto& options = hdr_context_.get_options();
	options.set(Rendering_context::Options::Render_analytical_lighting, render_analytical_lighting_);
	options.set(Rendering_context::Options::Render_image_based_lighting, render_image_based_lighting_);

	const auto& scene = app.get_scene();
	scene_renderer_->render(scene, hdr_context_);

	const auto& camera = hdr_context_.get_camera();
	hdr_filter_->set_exposure(camera.get_exposure());
	hdr_filter_->set_linear_white(camera.get_linear_white());

	post_processing_.render(hdr_target_->get_shader_resource_view(), hdr_context_.get_viewport(), presentation_context_);

	if (render_help_light_)
	{
		help_light_renderer_->render(scene, presentation_context_);
	}

	if (render_bounding_volumes_)
	{
		bounding_renderer_->render(scene, presentation_context_);
	}

	if (Application::Mode::Edit == app.get_mode())
	{
		editor_renderer_->render(scene, app.get_editor(), presentation_context_);
	}
/*
	Color4 clear_color(0.f, 0.f, 0.f, 0.f);
	device.clear_color(presentation_context_.get_framebuffer(), 1, &clear_color);

	device.clear_depth_stencil(presentation_context_.get_framebuffer(), 1.f, 0);

	const auto& scene = app.get_scene();
	scene_renderer_->render(scene, presentation_context_);
	*/
}

bool Renderer::get_render_analytical_lighting() const
{
	return render_analytical_lighting_;
}

void Renderer::set_render_analytical_lighting(bool enable)
{
	render_analytical_lighting_ = enable;
}

bool Renderer::get_render_image_based_lighting() const
{
	return render_image_based_lighting_;
}

void Renderer::set_render_image_based_lighting(bool enable)
{
	render_image_based_lighting_ = enable;
}


bool Renderer::get_render_bounding_volumes() const
{
	return render_bounding_volumes_;
}

void Renderer::set_render_bounding_volumes(bool enable)
{
	render_bounding_volumes_ = enable;
}

bool Renderer::get_render_help_light() const
{
	return render_help_light_;
}

void Renderer::set_render_help_light(bool enable)
{
	render_help_light_ = enable;
}

}
