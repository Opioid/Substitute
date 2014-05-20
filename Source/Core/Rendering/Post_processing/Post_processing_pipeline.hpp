#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Data_format.hpp"
#include "Resources/Handle.hpp"

class Resource_manager;

namespace rendering
{

class Shader_resource_view;
class Render_target_shader_resource_view;
class Post_processing_effect;

class Post_processing_pipeline : public Rendering_object
{

public:

	Post_processing_pipeline(Rendering_tool& rendering_tool);

	bool init(Resource_manager& resource_manager);

	bool resize_targets(const uint2& size, Data_format::Value format);

	void render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context);

	void set_hdr_effect(Post_processing_effect* hdr_effect);

	void push_back_effect(Post_processing_effect* effect);

private:

	Post_processing_effect* m_hdr_effect;

	Post_processing_effect* effect_;

	Rendering_context context_;

	Handle<Framebuffer> framebuffer_;

	Handle<Render_target_shader_resource_view> m_scratch_texture;
};

}


