#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Data_buffer.hpp"
#include <cstdint>

namespace rendering
{

class Rendering_tool;


class Deferred_volume
{

protected:

	Handle<Vertex_buffer> vertex_buffer_;
	Handle<Index_buffer>  index_buffer_;

	uint32_t num_indices_;
};

}
