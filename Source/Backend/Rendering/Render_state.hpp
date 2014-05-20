#pragma once

#include "Resources/Reference_counter.hpp"
#include <cstdint>

namespace rendering
{

class Rendering_device;

class Rasterizer_state : public Reference_counter
{

public:

	struct Description
	{
		enum class Cull_mode
		{

			None,
			Front,
			Back
		};

		Description();

		bool operator==(const Description& other) const;

		Cull_mode cull_mode;
		bool      front_ccw;
		bool      multisample_enable;
	};

	Rasterizer_state(const Description& description);

private:

	bool     cull_;
	uint32_t cull__face;
	uint32_t front_face_;
	bool     multisample_enable_;

	friend Rendering_device;
};

class Depth_stencil_state : public Reference_counter
{

public:

	struct Description
	{
		enum class Comparison
		{
			Never,
			Less,
			Equal,
			Less_equal,
			Greater,
			Not_equal,
			Greater_equal,
			Always
		};

		struct Stencil
		{
			enum class Stencil_op
			{
				Keep,
				Zero,
				Replace,
				Increment_saturate,
				Decrement_saturate,
				Invert,
				Increment,
				Decrement
			};

			bool operator==(const Stencil& other) const;

			Stencil_op fail_op		   = Stencil_op::Keep;
			Stencil_op depth_fail_op   = Stencil_op::Keep;
			Stencil_op pass_op		   = Stencil_op::Keep;
			Comparison comparison_func = Comparison::Never;
		};

		bool operator==(const Description& other) const;

		bool 	   depth_enable = false;
		bool 	   depth_write_mask = false;
		Comparison comparison_func = Comparison::Less;
		bool 	   stencil_enable = false;
		Stencil    front_face;
		Stencil    back_face;
	};

	Depth_stencil_state(const Description& description);

private:

	bool 	      depth_enable_;
	unsigned char depth_write_mask_;
	uint32_t      comparison_func_;
	bool 	      stencil_enable_;

	struct GL_stencil_op_description
	{
		GL_stencil_op_description(const Description::Stencil& stencil);

		uint32_t fail_op;
		uint32_t depth_fail_op;
		uint32_t pass_op;
		uint32_t comparison_func;
	};

	GL_stencil_op_description front_face_;
	GL_stencil_op_description back_face_;

	friend Rendering_device;
};

class Blend_state : public Reference_counter
{
	static const uint32_t s_num_render_targets = 4;

public:

	struct Description
	{
		enum class Blend
		{
			Zero,
			One,
			Source_color,
			Inverse_source_color,
			Source_alpha,
			Inverse_source_alpha,
			Destination_alpha,
			Inverse_destination_alpha,
			Destination_color,
			Inverse_destination_color,
			Source_alpha_saturated,
			Blend_factor,
			Inverse_blend_factor,
			Source1_color,
			Inverse_source1_color,
			Source1_alpha,
			Inverse_source1_alpha
		};

		enum class Blend_op
		{
			Add,
			Subract,
			Reverse_subtract,
			Min,
			Max
		};

		struct Color_write_mask
		{
			enum Value
			{
				Red   = 1,
				Green = 2,
				Blue  = 4,
				Alpha = 8,
				All   = Red | Green | Blue | Alpha
			};
		};

		struct Render_target
		{
			bool blend_enable = false;

			Blend    source_blend	   = Blend::One;
			Blend    destination_blend = Blend::Zero;
			Blend_op blend_op		   = Blend_op::Add;

			Blend    source_blend_alpha		 = Blend::One;
			Blend    destination_blend_alpha = Blend::Zero;
			Blend_op blend_op_alpha			 = Blend_op::Add;

			uint8_t  color_write_mask = Color_write_mask::All;
		};

		bool operator==(const Description& b) const;

		bool independent_blend_enable = false;
		Render_target render_targets[s_num_render_targets];
	};

	Blend_state(const Description& description);

private:

	struct GL_render_target
	{
		bool blend_enable;

		uint32_t source_blend;
		uint32_t destination_blend;
		uint32_t blend_op;
		uint32_t source_blend_alpha;
		uint32_t destination_blend_alpha;
		uint32_t blend_op_alpha;

		bool enable_red;
		bool enable_green;
		bool enable_blue;
		bool enable_alpha;
	};

	bool independent_blend_enable_;

	GL_render_target render_targets_[s_num_render_targets];

	friend Rendering_device;
};

}


