#include "Post_processing_effect.hpp"

namespace rendering
{

Post_processing_effect::Post_processing_effect(Rendering_tool& rendering_tool) : Rendering_object(rendering_tool)
{}

bool Post_processing_effect::resize_targets(const uint2& dimensions)
{
	return on_resize_targets(dimensions);
}

}
