#include "Directional_shadow_renderer_EVSM_MS.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Effect/Effect_provider.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Scene_rendering/Surface_collector.hpp"
#include "Rendering/Constant_buffer_cache.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"

namespace rendering
{

Directional_shadow_renderer_EVSM_MS::Directional_shadow_renderer_EVSM_MS(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& size) :
	Directional_shadow_renderer(rendering_tool, surface_collector, size)
{}

bool Directional_shadow_renderer_EVSM_MS::init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache)
{
    auto& device = rendering_tool_.device();

    Texture_description texture_description;

    texture_description.type = Texture_description::Type::Texture_2D;
    texture_description.format = Data_format::R32G32B32A32_Float;
    texture_description.dimensions.xy = shadow_map_dimensions_;
    texture_description.num_samples = 0;
    texture_description.shader_resource = true;

    shadow_map_ = device.create_render_target_shader_resource_view(texture_description);
    if (!shadow_map_)
    {
        return false;
    }

	if (!Shadow_renderer_EVSM_MS::init(shadow_map_dimensions_, rendering_tool_, resource_manager, constant_buffer_cache))
    {
        return false;
    }

    apply_shadow_.framebuffer = rendering_tool_.device().create_framebuffer();
    if (!apply_shadow_.framebuffer)
    {
        return false;
    }

	Flags<Effect_provider::Options, uint32_t> flags;
	flags.set(Effect_provider::Options::Use_custom_constant_buffers, true);

	apply_shadow_.effect = resource_manager.load<Effect>("Effects/Shadowing/Apply_shadow_map_EVSM.effect", flags.data());
    if (!apply_shadow_.effect)
    {
        return false;
    }

    apply_shadow_.technique        = apply_shadow_.effect->technique("Apply_shadow_map");
    apply_shadow_.volume_technique = apply_shadow_.effect->technique("Apply_shadow_map_volume");

    apply_shadow_.input_layout = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), apply_shadow_.technique->program()->signature());
    if (!apply_shadow_.input_layout)
    {
        return false;
    }

    apply_shadow_.volume_input_layout = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position3x32::vertex_layout_description(), apply_shadow_.volume_technique->program()->signature());
    if (!apply_shadow_.volume_input_layout)
    {
        return false;
    }

    if (!apply_shadow_.filter_kernel.init(apply_shadow_.effect, "Filter_kernel"))
    {
        return false;
    }

    Constant_buffer_adapter* change_per_camera_adapter = apply_shadow_.effect->constant_buffer_adapter("Change_per_camera");
    if (!change_per_camera_adapter)
    {
        return false;
    }

    Handle<Constant_buffer> change_per_camera_buffer = constant_buffer_cache.constant_buffer("Change_per_camera");
    if (!change_per_camera_buffer)
    {
        return false;
    }

    change_per_camera_adapter->set_constant_buffer(change_per_camera_buffer);

    if (!apply_shadow_.change_per_cascade.init(apply_shadow_.effect, "Change_per_cascade"))
    {
        return false;
    }

    apply_shadow_.effect->create_default_constant_buffers(device);

    if (!volume_.init(rendering_tool_))
    {
        return false;
    }

    for (uint32_t i = 0; i < filter_kernel_size_; ++i)
    {
        apply_shadow_.filter_kernel.data().filter_kernel[i] = filter_kernel_[i];
    }

    apply_shadow_.filter_kernel.update(device);

    return create_render_states();
}

Handle<Shader_resource_view>& Directional_shadow_renderer_EVSM_MS::shadow_map()
{
    return shadow_map_->shader_resource_view();
}

Handle<Shader_resource_view>& Directional_shadow_renderer_EVSM_MS::white_buffer()
{
    return white_buffer_->shader_resource_view();
}

void Directional_shadow_renderer_EVSM_MS::prepare_rendering(const Rendering_context& /*context*/)
{}

void Directional_shadow_renderer_EVSM_MS::render_cascade(const Rendering_context& context, const Cascade_data& cascade_data)
{
    generate_shadow(cascade_data.shadow_view_projection, surface_collector_.get_surfaces(), viewport_, rendering_tool_.device());

	resolve_shadow_linear(resolve_shadow_.framebuffer, depth_->shader_resource_view(), cascade_data.depth_clamp, viewport_, rendering_tool_);

    apply_shadow(context, cascade_data);
}

void Directional_shadow_renderer_EVSM_MS::apply_shadow(const Rendering_context& context, const Cascade_data& cascade_data)
{
    Rendering_device& device = rendering_tool_.device();

    device.set_viewports(1, &context.viewport());
    device.set_framebuffer(apply_shadow_.framebuffer);

    device.set_shader_resources(1, &deferred_depth_);
    device.set_shader_resources(1, &shadow_map_->shader_resource_view(), 1);

    apply_shadow_.effect->use(device);

    device.set_blend_state(apply_shadow_.blend_state);

    const auto& camera = context.camera();

    auto& change_per_cascade_data = apply_shadow_.change_per_cascade.data();
    change_per_cascade_data.shadow_transform = invert(camera.view()) * cascade_data.shadow_view_projection * scene::Light::texture_transform();
    change_per_cascade_data.depth_clamp = cascade_data.depth_clamp;

    if (cascade_data.last_cascade)
    {
        device.set_input_layout(apply_shadow_.input_layout);

        device.set_rasterizer_state(apply_shadow_.rasterizer_state);
        device.set_depth_stencil_state(apply_shadow_.ds_state, 1);

        apply_shadow_.change_per_cascade.update(device);

        apply_shadow_.technique->use();

        rendering_tool_.render_fullscreen_effect();
    }
    else
    {
        device.set_input_layout(apply_shadow_.volume_input_layout);

        device.set_rasterizer_state(camera.is_upside_down() ? apply_shadow_.rasterizer_state_upside_down : apply_shadow_.rasterizer_state);
        device.set_depth_stencil_state(apply_shadow_.volume_ds_state, 1);

        change_per_cascade_data.world = cascade_data.cascade_world;
        apply_shadow_.change_per_cascade.update(device);

        apply_shadow_.volume_technique->use();

        volume_.render(rendering_tool_);
    }
}

bool Directional_shadow_renderer_EVSM_MS::on_resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil)
{
    auto& cache = rendering_tool_.render_target_cache();

    Texture_description texture_description;
    texture_description.type = Texture_description::Type::Texture_2D;
    texture_description.dimensions.xy = dimensions;
    texture_description.format = Data_format::R8_UNorm;
    white_buffer_ = cache.get_render_target_shader_resource_view(texture_description);

    if (!white_buffer_)
    {
        return false;
    }

	apply_shadow_.framebuffer->set_render_targets(white_buffer_->render_target_view(), depth_stencil->depth_stencil_view());

    if (!apply_shadow_.framebuffer->is_valid())
    {
        return false;
    }

    return true;
}

bool Directional_shadow_renderer_EVSM_MS::create_render_states()
{
    Render_state_cache& cache = rendering_tool_.render_state_cache();

    Rasterizer_state::Description rasterizer_description;

    rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
    apply_shadow_.rasterizer_state = cache.get_rasterizer_state(rasterizer_description);
    if (!apply_shadow_.rasterizer_state)
    {
        return false;
    }

    resolve_shadow_.rasterizer_state = apply_shadow_.rasterizer_state;

    rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Front;
    apply_shadow_.rasterizer_state_upside_down = cache.get_rasterizer_state(rasterizer_description);
    if (!apply_shadow_.rasterizer_state_upside_down)
    {
        return false;
    }

    Depth_stencil_state::Description ds_description;

    ds_description.depth_enable = false;
    ds_description.depth_write_mask = false;
    ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;
    ds_description.stencil_enable = true;
    ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Decrement_saturate;
    ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
    ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Always;
    apply_shadow_.ds_state = cache.get_depth_stencil_state(ds_description);
    if (!apply_shadow_.ds_state)
    {
        return false;
    }

    ds_description.depth_enable = true;
    ds_description.depth_write_mask = false;
    ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Greater;
    ds_description.stencil_enable = true;
    ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Increment;
    ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
    ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
    ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Always;
    apply_shadow_.volume_ds_state = cache.get_depth_stencil_state(ds_description);
    if (!apply_shadow_.volume_ds_state)
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
    blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::Red;
    apply_shadow_.blend_state = cache.get_blend_state(blend_description);
    if (!apply_shadow_.blend_state)
    {
        return false;
    }

    return true;
}

}
