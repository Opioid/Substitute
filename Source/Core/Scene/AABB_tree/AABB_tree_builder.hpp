#pragma once

#include "Math/Intersection/AABB.hpp"
#include "Memory/Heap_cache.hpp"
#include <vector>

namespace scene
{

class AABB_tree;
class AABB_node;
class Static_prop;

class AABB_tree_builder
{

public:

    void build(AABB_tree& tree, const Heap_cache<Static_prop>& props);

private:

	struct Temp_aabb_node;

	void serialize(Temp_aabb_node* node);

	AABB_node* get_new_node();
	AABB_node* get_skip_node() const;

	AABB_node* nodes_;
	
	uint32_t num_nodes_;
	uint32_t current_node_;

    static void split(Temp_aabb_node* node, std::vector<Static_prop*>& props);
	static AABB calculate_AABB(const std::vector<Static_prop*>& props);
	
	struct Temp_aabb_node
	{
		Temp_aabb_node();
		~Temp_aabb_node();

		void get_num_sub_nodes(uint32_t& num_sub_nodes) const;

		AABB aabb;

		std::vector<Static_prop*> props;

		Temp_aabb_node* child0;
		Temp_aabb_node* child1;
	};
};

}
