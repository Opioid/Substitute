#include "Render_state.hpp"
#include <GL/glew.h>

namespace rendering
{

Rasterizer_state::Description::Description() :
	cull_mode(Cull_mode::None), front_ccw(false), multisample_enable(false)
{}

bool Rasterizer_state::Description::operator==(const Description& other) const
{
	return cull_mode          == other.cull_mode
		&& front_ccw          == other.front_ccw
		&& multisample_enable == other.multisample_enable;
}

Rasterizer_state::Rasterizer_state(const Description& description) :
	cull_(Description::Cull_mode::None != description.cull_mode),
	cull__face(Description::Cull_mode::Front == description.cull_mode ? GL_FRONT : GL_BACK),
	front_face_(description.front_ccw ? GL_CCW : GL_CW),
	multisample_enable_(description.multisample_enable)
{}

bool Depth_stencil_state::Description::Stencil::operator==(const Stencil& other) const
{
	return fail_op         == other.fail_op
		&& depth_fail_op   == other.depth_fail_op
		&& pass_op         == other.pass_op
		&& comparison_func == other.comparison_func;
}

bool Depth_stencil_state::Description::operator==(const Description& other) const
{
	return depth_enable     == other.depth_enable
		&& depth_write_mask == other.depth_write_mask
		&& comparison_func  == other.comparison_func
		&& stencil_enable   == other.stencil_enable
		&& front_face       == other.front_face
		&& back_face        == other.back_face;
}

uint32_t g_ogl_comparison_func[] =
{
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};

uint32_t g_ogl_stencil_op[] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INVERT,
	GL_INCR_WRAP,
	GL_DECR_WRAP
};

Depth_stencil_state::Depth_stencil_state(const Description& description) :
	depth_enable_(description.depth_enable),
	depth_write_mask_(description.depth_write_mask ? GL_TRUE : GL_FALSE),
	comparison_func_(g_ogl_comparison_func[static_cast<uint32_t>(description.comparison_func)]),
	stencil_enable_(description.stencil_enable),
	front_face_(description.front_face),
	back_face_(description.back_face)
{}

Depth_stencil_state::GL_stencil_op_description::GL_stencil_op_description(const Description::Stencil& stencil) :
	fail_op(		g_ogl_stencil_op[static_cast<uint32_t>(stencil.fail_op)]),
	depth_fail_op(  g_ogl_stencil_op[static_cast<uint32_t>(stencil.depth_fail_op)]),
	pass_op(		g_ogl_stencil_op[static_cast<uint32_t>(stencil.pass_op)]),
	comparison_func(g_ogl_comparison_func[static_cast<uint32_t>(stencil.comparison_func)])
{}

bool Blend_state::Description::operator==(const Description& other) const
{
	if (independent_blend_enable != other.independent_blend_enable)
	{
		return false;
	}

	uint32_t num_compare_targets = independent_blend_enable ? s_num_render_targets : 1;

	for (uint32_t i = 0; i < num_compare_targets; ++i)
	{
		if (render_targets[i].blend_enable != other.render_targets[i].blend_enable
		||  render_targets[i].source_blend != other.render_targets[i].source_blend
		||  render_targets[i].destination_blend != other.render_targets[i].destination_blend
		||  render_targets[i].blend_op != other.render_targets[i].blend_op
		||  render_targets[i].source_blend_alpha != other.render_targets[i].source_blend_alpha
		||  render_targets[i].destination_blend_alpha != other.render_targets[i].destination_blend_alpha
		||  render_targets[i].blend_op_alpha != other.render_targets[i].blend_op_alpha
		||  render_targets[i].color_write_mask != other.render_targets[i].color_write_mask)
		{
			return false;
		}
	}

	return true;
}

uint32_t g_ogl_blend[] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_SRC1_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC1_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA
};

uint32_t g_ogl_blend_op[] =
{
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

Blend_state::Blend_state(const Description& description) :
	independent_blend_enable_(description.independent_blend_enable)
{
	for (uint32_t i = 0; i < s_num_render_targets; ++i)
	{
		render_targets_[i].blend_enable = description.render_targets[i].blend_enable;

		render_targets_[i].source_blend      = g_ogl_blend[static_cast<uint32_t>(description.render_targets[i].source_blend)];
		render_targets_[i].destination_blend = g_ogl_blend[static_cast<uint32_t>(description.render_targets[i].destination_blend)];
		render_targets_[i].blend_op          = g_ogl_blend_op[static_cast<uint32_t>(description.render_targets[i].blend_op)];

		render_targets_[i].source_blend_alpha      = g_ogl_blend[static_cast<uint32_t>(description.render_targets[i].source_blend_alpha)];
		render_targets_[i].destination_blend_alpha = g_ogl_blend[static_cast<uint32_t>(description.render_targets[i].destination_blend_alpha)];
		render_targets_[i].blend_op_alpha          = g_ogl_blend_op[static_cast<uint32_t>(description.render_targets[i].blend_op_alpha)];

		render_targets_[i].enable_red   = Description::Color_write_mask::Red   == (description.render_targets[i].color_write_mask & Description::Color_write_mask::Red);
		render_targets_[i].enable_green = Description::Color_write_mask::Green == (description.render_targets[i].color_write_mask & Description::Color_write_mask::Green);
		render_targets_[i].enable_blue  = Description::Color_write_mask::Blue  == (description.render_targets[i].color_write_mask & Description::Color_write_mask::Blue);
		render_targets_[i].enable_alpha = Description::Color_write_mask::Alpha == (description.render_targets[i].color_write_mask & Description::Color_write_mask::Alpha);
	}
}

}
