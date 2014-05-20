#include "AABB_tree.hpp"
#include "AABB_node.hpp"

AABB_tree::AABB_tree() : root(nullptr), num_geometry_clusters(0)
{}

AABB_tree::~AABB_tree()
{
	delete root;
}
