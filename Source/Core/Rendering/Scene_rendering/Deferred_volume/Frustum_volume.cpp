#include "Frustum_volume.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Math/Intersection/Frustum.hpp"

namespace rendering
{

bool Frustum_volume::init(Rendering_tool& rendering_tool)
{
	vertex_buffer_ = rendering_tool.device().create_vertex_buffer(sizeof(float3) * 8);
	if (!vertex_buffer_)
	{
		return false;
	}

	num_indices_ = 36;
	unsigned short indices[36];

	//bottom
	indices[0]  = 0;
	indices[1]  = 5;
	indices[2]  = 4;
	indices[3]  = 0;
	indices[4]  = 1;
	indices[5]  = 5;

	//top
	indices[6]  = 2;
	indices[7]  = 6;
	indices[8]  = 7;
	indices[9]  = 2;
	indices[10] = 7;
	indices[11] = 3;

	//left
	indices[12] = 0;
	indices[13] = 4;
	indices[14] = 6;
	indices[15] = 0;
	indices[16] = 6;
	indices[17] = 2;

	//right
	indices[18] = 1;
	indices[19] = 7;
	indices[20] = 5;
	indices[21] = 1;
	indices[22] = 3;
	indices[23] = 7;

	//near
	indices[24] = 0;
	indices[25] = 2;
	indices[26] = 1;
	indices[27] = 1;
	indices[28] = 2;
	indices[29] = 3;

	//far
	indices[30] = 4;
	indices[31] = 5;
	indices[32] = 6;
	indices[33] = 5;
	indices[34] = 7;
	indices[35] = 6;

	index_buffer_ = rendering_tool.device().create_index_buffer(sizeof(unsigned short) * num_indices_, indices, Data_format::R16_UInt);
	return index_buffer_ != nullptr;
}

void Frustum_volume::setup(const float3& pos, const Frustum& frustum, Rendering_tool& rendering_tool)
{
	rendering_tool.device().update_buffer(*vertex_buffer_, 0, 8 * sizeof(float3), frustum.points_);
}

void Frustum_volume::render(Rendering_tool& rendering_tool)
{
	auto& device = rendering_tool.device();

	device.set_primitive_topology(Primitive_topology::Triangle_list);

	device.set_vertex_buffer(vertex_buffer_, sizeof(float3));
	device.set_index_buffer(index_buffer_);

	device.draw_indexed(num_indices_);
}

}
