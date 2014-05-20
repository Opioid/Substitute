#include "Cascade_volume.hpp"
#include "../../Rendering_tool.hpp"

namespace rendering
{

bool Cascade_volume::init(Rendering_tool& rendering_tool)
{
	float3 vertices[8];

	vertices[0] = float3(-0.5f, -0.5f, 0.f);
	vertices[1] = float3(-0.5f,  0.5f, 0.f);
	vertices[2] = float3( 0.5f,  0.5f, 0.f);
	vertices[3] = float3( 0.5f, -0.5f, 0.f);

	vertices[4] = float3(-0.5f, -0.5f, 1.f);
	vertices[5] = float3(-0.5f,  0.5f, 1.f);
	vertices[6] = float3( 0.5f,  0.5f, 1.f);
	vertices[7] = float3( 0.5f, -0.5f, 1.f);

	vertex_buffer_ = rendering_tool.get_device().create_vertex_buffer(sizeof(float3) * 8, vertices);
	if (!vertex_buffer_)
	{
		return false;
	}

	num_indices_ = 36;
	unsigned short indices[36];

	// The indices are chosen so that the triangles have their front point IN to the volume
	// front
	indices[0]  = 1;
	indices[1]  = 0;
	indices[2]  = 2;
	indices[3]  = 3;
	indices[4]  = 2;
	indices[5]  = 0;

	// back
	indices[6]  = 4;
	indices[7]  = 5;
	indices[8]  = 6;
	indices[9]  = 4;
	indices[10] = 6;
	indices[11] = 7;

	// left
	indices[12] = 0;
	indices[13] = 1;
	indices[14] = 4;
	indices[15] = 4;
	indices[16] = 1;
	indices[17] = 5;

	// right
	indices[18] = 2;
	indices[19] = 3;
	indices[20] = 6;
	indices[21] = 6;
	indices[22] = 3;
	indices[23] = 7;

	// top
	indices[24] = 1;
	indices[25] = 2;
	indices[26] = 5;
	indices[27] = 5;
	indices[28] = 2;
	indices[29] = 6;

	// bottom
	indices[30] = 3;
	indices[31] = 0;
	indices[32] = 4;
	indices[33] = 3;
	indices[34] = 4;
	indices[35] = 7;

	index_buffer_ = rendering_tool.get_device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

void Cascade_volume::render(Rendering_tool& rendering_tool) const
{
	auto& device = rendering_tool.get_device();

	device.set_primitive_topology(Primitive_topology::Triangle_list);

	device.set_vertex_buffer(vertex_buffer_, sizeof(float3));
	device.set_index_buffer(index_buffer_);

	device.draw_indexed(num_indices_);
}

}
