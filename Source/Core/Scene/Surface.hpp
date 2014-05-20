#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Data_buffer.hpp"
#include "Material.hpp"
#include <cstdint>

template<typename T> struct tMatrix4x4;
typedef tMatrix4x4<float> float4x4;

namespace rendering
{

class Vertex_layout_description;
class Vertex_buffer;
class Index_buffer;

}

namespace scene
{

struct Surface
{
    void clear();

	const rendering::Vertex_layout_description* vd;

	uint32_t start_index;
	uint32_t num_indices;

	const Handle<rendering::Vertex_buffer>* vertex_buffers;
	Handle<rendering::Index_buffer> index_buffer;

	const float4x4* world;

	Handle<Material> material;
};

}

