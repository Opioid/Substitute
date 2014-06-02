#pragma once

#include "AABB_node.hpp"
#include <vector>
#include <cstdint>

namespace scene
{

class AABB_node;
class Static_prop;

class AABB_tree
{

public:

	AABB_tree();
	~AABB_tree();

    void clear();

	bool is_empty() const;

	const AABB_node* root() const;

	const std::vector<Static_prop*>& static_props() const;

	AABB_node::Geometry* allocate_geometry_clusters(uint32_t num_geometry_clusters);
	AABB_node*           allocate_nodes(uint32_t num_nodes);

	void add_static_prop(Static_prop* prop);

private:

	bool add_static_prop(Static_prop* prop, AABB_node* node);

	uint32_t             num_geometry_clusters_;
	AABB_node::Geometry* geometry_clusters_;
	uint32_t             geometry_clusters_size_;

	uint32_t   num_nodes_;
	AABB_node* nodes_;
    uint32_t   nodes_size_;

	std::vector<Static_prop*> props_;

	friend AABB_tree_builder;
};

}
