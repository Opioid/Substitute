#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"

class Resource_manager;

namespace rendering
{

struct Viewport;
class Rendering_context;
class Shader_resource_view;

class Post_processing_effect : public Rendering_object
{

public:

	Post_processing_effect(Rendering_tool& rendering_tool);

	virtual bool init(Resource_manager& resource_manager) = 0;

	bool resize_targets(const uint2& dimensions);

	virtual void render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context) = 0;

private:

	virtual bool on_resize_targets(const uint2& dimensions) = 0;
};

}


