#pragma once

#include "Math/Intersection/AABB.hpp"
#include "Rendering/Vertex_format.hpp"
#include <vector>

class Material;

struct AABB_node
{
	AABB_node();
	~AABB_node();

	void get_num_sub_nodes(uint32_t& num_sub_nodes) const;

	struct Geometry
	{
		Geometry();
		~Geometry();

		struct Group
		{
			uint32_t material_id;

			uint32_t start_index;
			uint32_t num_indices;
		};

		uint32_t num_groups;
		Group*   groups;

		uint32_t num_vertices;
		rendering::Vertex_position3x32_tex_coord2x32_normal1010102* vertices;

		/*
		float3* positions;
		float2* texCoords;
		Vector4_UNorm1010102* normals;
		*/

		uint32_t        num_indices;
		unsigned short* indices;
	};

	Geometry* geometry;

	AABB aabb;

	uint32_t index;
	uint32_t geometry_index;
	uint32_t skip_offset;

	AABB_node* child0;
	AABB_node* child1;
};
