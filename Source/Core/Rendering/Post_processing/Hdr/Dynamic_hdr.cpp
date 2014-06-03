#include "Dynamic_hdr.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"
#include "Math/Vector.inl"
#include "Logging/Logging.hpp"

namespace rendering
{

Dynamic_hdr::Dynamic_hdr(Rendering_tool& rendering_tool, Data_format::Value hdr_format) : Hdr_filter(rendering_tool, hdr_format)
{}

bool Dynamic_hdr::init(Resource_manager& resource_manager)
{
	effect_ = resource_manager.load<Effect>("Effects/Hdr/Static_hdr.effect");
	if (!effect_)
	{
		return false;
	}

	compose_technique_         = effect_->technique("Compose");
	luminance_technique_       = effect_->technique("Luminance");
	scale_down_technique_      = effect_->technique("Scale_down");
	bright_pass_technique_     = effect_->technique("Bright_pass");
	blur_horizontal_technique_ = effect_->technique("Blur_horizontal");
	blur_vertical_technique_   = effect_->technique("Blur_vertical");

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), effect_->technique(0)->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_scene_.init(effect_, "Change_per_scene"))
	{
		return false;
	}

	if (!filter_kernel_.init(effect_, "Filter_kernel"))
	{
		return false;
	}

	return create_render_states();
}

void Dynamic_hdr::render(const Handle<Shader_resource_view>& source, const Viewport& /*source_viewport*/, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.device();

	device.set_input_layout(input_layout_);

	device.set_shader_resources(1, &source);
	effect_->use(device);

	device.set_depth_stencil_state(ds_state_);

	calculate_average_luminance();

	device.set_shader_resources(1, &luminance_target_->shader_resource_view(), 1);

	bloom();

	device.set_framebuffer(context.framebuffer());
	device.set_viewports(1, &context.viewport());

	device.set_blend_state(compose_blend_state_);

	device.set_shader_resources(1, &bloom_targets_[0]->shader_resource_view(), 2);

	change_per_scene_.data().linear_white = linear_white_;
	change_per_scene_.update(device);

	compose_technique_->use();

	rendering_tool_.render_fullscreen_effect();
}

bool Dynamic_hdr::on_resize_targets(const uint2& dimensions)
{
	auto& cache = rendering_tool_.render_target_cache();

	uint2 luminance_dimensions = uint2(1024, 1024);

	luminance_viewport_ = Viewport(float2(0.f, 0.f), float2(luminance_dimensions));

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.dimensions.xy = luminance_dimensions;
	texture_description.num_mip_levels = Texture_description::calculate_num_mip_levels(texture_description.dimensions);
	texture_description.format = Data_format::R16_Float;
	luminance_target_ = cache.render_target_shader_resource_view(texture_description);

	if (!luminance_target_)
	{
		return false;
	}

	luminance_framebuffer_ = rendering_tool_.device().create_framebuffer();

	if (!luminance_framebuffer_)
	{
		return false;
	}

	luminance_framebuffer_->set_render_targets(luminance_target_->render_target_view());

	if (!luminance_framebuffer_->is_valid())
	{
		return false;
	}

	uint2 bloom_dimensions = dimensions / 2;

	bloom_viewport_ = Viewport(float2(0.f, 0.f), float2(bloom_dimensions));

	texture_description.dimensions.xy = bloom_dimensions;
	texture_description.num_mip_levels = 1;
	texture_description.format = hdr_format_;

	bloom_targets_[0] = cache.render_target_shader_resource_view(texture_description);
	if (!bloom_targets_[0])
	{
		return false;
	}

	bloom_targets_[1] =  cache.render_target_shader_resource_view(texture_description, { bloom_targets_[0] });
	if (!bloom_targets_[1])
	{
		return false;
	}

	for (uint32_t i = 0; i < 2; ++i)
	{
		bloom_framebuffers_[i] = rendering_tool_.device().create_framebuffer();

		if (!bloom_framebuffers_[i])
		{
			return false;
		}

		bloom_framebuffers_[i]->set_render_targets(bloom_targets_[i]->render_target_view());

		if (!bloom_framebuffers_[i]->is_valid())
		{
			return false;
		}
	}

	prepare_filter_kernel(bloom_dimensions);

	return true;
}

void Dynamic_hdr::calculate_average_luminance()
{
	Rendering_device& device = rendering_tool_.device();

	device.set_viewports(1, &luminance_viewport_);

	device.set_framebuffer(luminance_framebuffer_);

	device.set_blend_state(luminance_blend_state_);

	luminance_technique_->use();

	rendering_tool_.render_fullscreen_effect();

	device.generate_mip_maps(luminance_target_->shader_resource_view()->texture());
}

void Dynamic_hdr::bloom()
{
	Rendering_device& device = rendering_tool_.device();

	device.set_viewports(1, &bloom_viewport_);

	device.set_blend_state(bloom_blend_state_);

	device.set_framebuffer(bloom_framebuffers_[0]);
	scale_down_technique_->use();
	rendering_tool_.render_fullscreen_effect();
/*
	device.set_framebuffer(bloom_framebuffers_[1]);
	device.set_shader_resources(1, &bloom_targets_[0]->shader_resource_view(), 2);
	bright_pass_technique_->use();
	rendering_tool_.render_fullscreen_effect();
*/
	device.set_framebuffer(bloom_framebuffers_[1]);
	device.set_shader_resources(1, &bloom_targets_[0]->shader_resource_view(), 3);
	blur_horizontal_technique_->use();
	rendering_tool_.render_fullscreen_effect();

	device.set_framebuffer(bloom_framebuffers_[0]);
	device.set_shader_resources(1, &bloom_targets_[1]->shader_resource_view(), 3);
	blur_vertical_technique_->use();
	rendering_tool_.render_fullscreen_effect();
}

void Dynamic_hdr::prepare_filter_kernel(const uint2& dimensions)
{
	auto& filter_kernel_data = filter_kernel_.data();
	filter_kernel_data.texel_size = 1.f / float2(dimensions);

	const float2 sample_offset = 0.5f * filter_kernel_data.texel_size;
	const float weight = 1.f / 4.f;
	filter_kernel_data.filter_kernel[0] = float4(-sample_offset.x,  sample_offset.y, weight, 0.f);
	filter_kernel_data.filter_kernel[1] = float4( sample_offset.x,  sample_offset.y, weight, 0.f);
	filter_kernel_data.filter_kernel[2] = float4(-sample_offset.x, -sample_offset.y, weight, 0.f);
	filter_kernel_data.filter_kernel[3] = float4( sample_offset.x, -sample_offset.y, weight, 0.f);

	filter_kernel_.update(rendering_tool_.device());
}

bool Dynamic_hdr::create_render_states()
{
	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.stencil_enable = false;

	ds_state_ = rendering_tool_.render_state_cache().depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::All;

	compose_blend_state_ = rendering_tool_.render_state_cache().blend_state(blend_description);
	if (!compose_blend_state_)
	{
		return false;
	}

	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red;

	luminance_blend_state_ = rendering_tool_.render_state_cache().blend_state(blend_description);
	if (!luminance_blend_state_)
	{
		return false;
	}

	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	bloom_blend_state_ = rendering_tool_.render_state_cache().blend_state(blend_description);
	if (!bloom_blend_state_)
	{
		return false;
	}

	return true;
}

}
/*#include "../Render_tool.hpp"
#include "../Render_context.hpp"
#include "../../Resources/Resource_manager.hpp"
#include "Rendering/VertexFormat.hpp"
#include "Rendering/Color.hpp"


Standard_Hdr::Standard_Hdr(Render_tool& rendering_tool) : HdrFilter(rendering_tool)
{}


Standard_Hdr::~Standard_Hdr()
{}


bool Standard_Hdr::init(const uint2& size, Data_format::Value format, Resource_manager& resource_manager)
{
	effect_ = resource_manager.load<Effect>("Effects/Hdr/Standard_Hdr.fx");
	if (!effect_)
	{
		return false;
	}

	m_size_div4 = size / 4;

	m_targets[0] = m_render_tool.getScratchTexturePool().texture(m_size_div4, format);

	std::list<const Render_target_shader_resource_view*> must_not_match;
	must_not_match.push_back(m_targets[0]);
	m_targets[1] = m_render_tool.getScratchTexturePool().texture(m_size_div4, format, false, &must_not_match);

	m_luminance_size = uint2(1024, 1024);
	m_luminance = m_render_tool.getScratchTexturePool().texture(m_luminance_size, Data_format::R16_Float, true);

	layout_ = m_render_tool.getInputLayoutManager().getInputLayout(Vertex_Pos3x32_Tex2x32::vertex_layout_description(), effect_, "Render");
	if (!layout_)
	{
		return false;
	}

	return create_render_states();
}


void Standard_Hdr::resolve(const Render_context& context, const Shader_resource_view* source, const float speed)
{
	m_render_tool.get_context().set_depth_stencil_state(ds_state_);

	m_render_tool.get_context().set_input_layout(layout_);



	calculate_average_luminance(source, context.viewport_.size_);

	bloom(source, context.viewport_.size_);

	m_render_tool.get_context().set_blend_state(blend_state_);

	m_render_tool.get_context().set_render_target(context.m_target, nullptr);
	m_render_tool.get_context().set_viewport(context.viewport_);

//	effect_->setResource("g_source", &m_targets[1]->shader_resource_view());
//	effect_->technique("Render").apply(m_render_tool.get_context());

	effect_->setFloat("g_linear_white", linear_white_);

	effect_->setResource("g_source", source);
	effect_->setResource("g_average_luminance", &m_luminance->shader_resource_view());
	effect_->setResource("g_bloom", &m_targets[1]->shader_resource_view());
	effect_->technique("Compose").apply(m_render_tool.get_context());

	m_render_tool.drawFullscreenTriangle();
}


void Standard_Hdr::calculate_average_luminance(const Shader_resource_view* source, const uint2& size)
{
	Viewport viewport(m_luminance_size);
	m_render_tool.get_context().set_viewport(viewport);

	m_render_tool.get_context().set_blend_state(blend_state__luminance);

	m_render_tool.get_context().set_render_target(&m_luminance->Render_target_view(), nullptr);

	effect_->setResource("g_source", source);

	effect_->technique("Luminance").apply(m_render_tool.get_context());

	m_render_tool.drawFullscreenTriangle();

	m_render_tool.get_context().generateMips(&m_luminance->shader_resource_view());
}


void Standard_Hdr::bloom(const Shader_resource_view* source, const uint2& size)
{
	Viewport viewport(m_size_div4);
	m_render_tool.get_context().set_viewport(viewport);

	m_render_tool.get_context().set_blend_state(blend_state__bloom);

	m_render_tool.get_context().set_render_target(&m_targets[0]->Render_target_view(), nullptr);

	float2 texel_size(1.f / float(size.x), 1.f / float(size.y));

	prepare_down_sample_kernel4x4(texel_size, true);

	effect_->setResource("g_source", source);

	effect_->technique("Scale_down").apply(m_render_tool.get_context());
	m_render_tool.drawFullscreenTriangle();
	
	texel_size = float2(1.f / float(m_size_div4.x), 1.f / float(m_size_div4.y));

	effect_->setVector("g_texel_size", texel_size);

	m_render_tool.get_context().set_render_target(&m_targets[1]->Render_target_view(), nullptr);
	effect_->setResource("g_source", &m_targets[0]->shader_resource_view());
	effect_->setResource("g_average_luminance", &m_luminance->shader_resource_view());
	effect_->technique("Bright_filter").apply(m_render_tool.get_context());
	m_render_tool.drawFullscreenTriangle();
	
	m_render_tool.get_context().unbindShaderResources();

	m_render_tool.get_context().set_render_target(&m_targets[0]->Render_target_view(), nullptr);
	effect_->setResource("g_source", &m_targets[1]->shader_resource_view());
	effect_->technique("Blur_horizontal").apply(m_render_tool.get_context());
	m_render_tool.drawFullscreenTriangle();

	m_render_tool.get_context().unbindShaderResources();

	m_render_tool.get_context().set_render_target(&m_targets[1]->Render_target_view(), nullptr);
	effect_->setResource("g_source", &m_targets[0]->shader_resource_view());
	effect_->technique("Blur_vertical").apply(m_render_tool.get_context());
	m_render_tool.drawFullscreenTriangle();
}


void Standard_Hdr::prepare_down_sample_kernel4x4(const float2& texel_size, bool bilinear)
{
	if (bilinear)
	{
		m_filter_kernel[0] = float2(-texel_size.x,  texel_size.y);
		m_filter_kernel[1] = float2( texel_size.x,  texel_size.y);
		m_filter_kernel[2] = float2(-texel_size.x, -texel_size.y);
		m_filter_kernel[3] = float2( texel_size.x, -texel_size.y);

		effect_->setVectors("g_filter_kernel", m_filter_kernel, 4);
		effect_->setInt("g_filter_size", 4);
		effect_->setFloat("g_filter_size_inv", 1.f / 4.f);
	}
	else
	{
		float2 halfTexelSize = 0.5f * texel_size;

		m_filter_kernel[0]  = float2(-halfTexelSize.x - texel_size.x, halfTexelSize.y + texel_size.y);
		m_filter_kernel[1]  = float2(-halfTexelSize.x,               halfTexelSize.y + texel_size.y);
		m_filter_kernel[2]  = float2( halfTexelSize.x,               halfTexelSize.y + texel_size.y);
		m_filter_kernel[3]  = float2( halfTexelSize.x + texel_size.x, halfTexelSize.y + texel_size.y);

		m_filter_kernel[4]  = float2(-halfTexelSize.x - texel_size.x, halfTexelSize.y);
		m_filter_kernel[5]  = float2(-halfTexelSize.x,               halfTexelSize.y);
		m_filter_kernel[6]  = float2( halfTexelSize.x,               halfTexelSize.y);
		m_filter_kernel[7]  = float2( halfTexelSize.x + texel_size.x, halfTexelSize.y);

		m_filter_kernel[8]  = float2(-halfTexelSize.x - texel_size.x, -halfTexelSize.y);
		m_filter_kernel[9]  = float2(-halfTexelSize.x,               -halfTexelSize.y);
		m_filter_kernel[10] = float2( halfTexelSize.x,               -halfTexelSize.y);
		m_filter_kernel[11] = float2( halfTexelSize.x + texel_size.x, -halfTexelSize.y);

		m_filter_kernel[12] = float2(-halfTexelSize.x - texel_size.x, -halfTexelSize.y - texel_size.y);
		m_filter_kernel[13] = float2(-halfTexelSize.x,               -halfTexelSize.y - texel_size.y);
		m_filter_kernel[14] = float2( halfTexelSize.x,               -halfTexelSize.y - texel_size.y);
		m_filter_kernel[15] = float2( halfTexelSize.x + texel_size.x, -halfTexelSize.y - texel_size.y);

		effect_->setVectors("g_filter_kernel", m_filter_kernel, 16);
		effect_->setInt("g_filter_size", 16);
		effect_->setFloat("g_filter_size_inv", 1.f / 16.f);
	}
}


bool Standard_Hdr::create_render_states()
{
	D3D11_DEPTH_STENCIL_DESC ds_desc;
	ZeroMemory(&ds_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	ds_desc.DepthEnable    = false;
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ds_desc.StencilEnable  = false;

	ds_state_ = m_render_tool.device().create_depth_stencil_state(ds_desc);
	if (!ds_state_)
	{
		return false;
	}

	D3D11_BLEND_DESC blend_desc;
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blend_state_ = m_render_tool.device().create_blend_state(blend_desc);
	if (!blend_state_)
	{
		return false;
	}

	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;

	blend_state__luminance = m_render_tool.device().create_blend_state(blend_desc);
	if (!blend_state__luminance)
	{
		return false;
	}

	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	blend_state__bloom = m_render_tool.device().create_blend_state(blend_desc);
	if (!blend_state__bloom)
	{
		return false;
	}

	return true;
}
*/
