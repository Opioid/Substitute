#pragma once

namespace rendering
{

class Rendering_tool;
class Rendering_context;
class Framebuffer;
class Effect;
class Effect_technique;
class Input_layout;
class Rasterizer_state;
class Depth_stencil_state;
class Blend_state;
class Shader_resource_view;
class Render_target_shader_resource_view;
class Depth_stencil_shader_resource_view;
class Depth_stencil_view;

class Rendering_object
{

public:

	Rendering_object(Rendering_tool& rendering_tool);
	virtual ~Rendering_object();

protected:

	Rendering_tool& rendering_tool_;
};

}


