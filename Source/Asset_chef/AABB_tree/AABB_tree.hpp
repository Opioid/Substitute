#pragma once

#include <cstdint>

struct AABB_node;

struct AABB_tree
{
	AABB_tree();
	~AABB_tree();

	AABB_node* root;

	uint32_t num_geometry_clusters;
};
