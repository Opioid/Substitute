#pragma once

#include "AABB_node.hpp"
#include <string>
#include <cstdint>

struct AABB_tree;

class AABB_tree_writer
{

public:

	bool write(const std::string& name, const AABB_tree& tree);

private:

	void write_geometry_clusters(const AABB_tree& tree, std::ofstream& stream);
	void write_geometry_cluster(AABB_node* node, std::ofstream& stream);

	void write(AABB_node* node, std::ofstream& stream);

	uint32_t num_nodes_;
	uint32_t current_node_;
};
