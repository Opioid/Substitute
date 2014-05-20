#include "AABB_node.hpp"

AABB_node::AABB_node() : geometry_index(0xFFFFFFFF), skip_offset(0), geometry(nullptr), child0(nullptr), child1(nullptr)
{}

AABB_node::~AABB_node()
{
	delete geometry;

	delete child0;
	delete child1;
}

void AABB_node::get_num_sub_nodes(uint32_t& num_sub_nodes) const
{
	if (child0)
	{
		num_sub_nodes += 2;

		child0->get_num_sub_nodes(num_sub_nodes);
		child1->get_num_sub_nodes(num_sub_nodes);
	}
}

AABB_node::Geometry::Geometry() : num_groups(0), groups(nullptr), num_vertices(0), vertices(nullptr), num_indices(0), indices(nullptr)
{}

AABB_node::Geometry::~Geometry()
{
	delete [] vertices;

	delete [] indices;

	delete [] groups;
}
