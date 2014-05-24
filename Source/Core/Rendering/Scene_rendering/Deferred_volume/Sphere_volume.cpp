#include "Sphere_volume.hpp"
#include "../../Rendering_tool.hpp"


namespace rendering
{

bool Sphere_volume::init(Rendering_tool& rendering_tool)
{
	float3 vertices[8];

	vertices[0] = float3(-1.f, -1.f, -1.f);
	vertices[1] = float3(-1.f,  1.f, -1.f);
	vertices[2] = float3( 1.f,  1.f, -1.f);
	vertices[3] = float3( 1.f, -1.f, -1.f);

	vertices[4] = float3(-1.f, -1.f, 1.f);
	vertices[5] = float3(-1.f,  1.f, 1.f);
	vertices[6] = float3( 1.f,  1.f, 1.f);
	vertices[7] = float3( 1.f, -1.f, 1.f);

	vertex_buffer_ = rendering_tool.device().create_vertex_buffer(sizeof(float3) * 8, vertices);
	if (!vertex_buffer_)
	{
		return false;
	}

	num_indices_ = 36;
	unsigned short indices[36];

	// front
	indices[0]  = 0;
	indices[1]  = 1;
	indices[2]  = 2;
	indices[3]  = 2;
	indices[4]  = 3;
	indices[5]  = 0;

	// back
	indices[6]  = 5;
	indices[7]  = 4;
	indices[8]  = 6;
	indices[9]  = 6;
	indices[10] = 4;
	indices[11] = 7;

	// left
	indices[12] = 0;
	indices[13] = 4;
	indices[14] = 1;
	indices[15] = 1;
	indices[16] = 4;
	indices[17] = 5;

	// right
	indices[18] = 2;
	indices[19] = 6;
	indices[20] = 3;
	indices[21] = 3;
	indices[22] = 6;
	indices[23] = 7;

	// top
	indices[24] = 1;
	indices[25] = 5;
	indices[26] = 2;
	indices[27] = 2;
	indices[28] = 5;
	indices[29] = 6;

	// bottom
	indices[30] = 0;
	indices[31] = 3;
	indices[32] = 4;
	indices[33] = 4;
	indices[34] = 3;
	indices[35] = 7;

	index_buffer_ = rendering_tool.device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

void Sphere_volume::render(Rendering_tool& rendering_tool) const
{
	auto& device = rendering_tool.device();

	device.set_primitive_topology(Primitive_topology::Triangle_list);

	device.set_vertex_buffer(vertex_buffer_, sizeof(float3));
	device.set_index_buffer(index_buffer_);

	device.draw_indexed(num_indices_);
}

}
