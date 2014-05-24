#include "AABB_tree_builder.hpp"
#include "AABB_tree.hpp"
#include "AABB_node.hpp"
#include "../Static_prop.hpp"

namespace scene
{

void AABB_tree_builder::build(AABB_tree& tree, const Heap_cache<Static_prop>& props)
{
    std::vector<Static_prop*> temp_props(props.size());

    props.copy(temp_props);

	Temp_aabb_node root;
    split(&root, temp_props);

	num_nodes_ = 1;
	root.get_num_sub_nodes(num_nodes_);

    nodes_ = tree.allocate_nodes(num_nodes_);

	current_node_ = 0;

	serialize(&root);
}

void AABB_tree_builder::serialize(Temp_aabb_node* node)
{
	AABB_node* n = get_new_node();

	n->aabb_ = node->aabb;
	n->props_.swap(node->props);

	if (node->child0)
	{
		serialize(node->child0);

		serialize(node->child1);

		n->set_has_children(true);
	}

	n->set_skip_node(get_skip_node());
}

AABB_node* AABB_tree_builder::get_new_node()
{
	return &nodes_[current_node_++];
}

AABB_node* AABB_tree_builder::get_skip_node() const
{
	if (current_node_ >= num_nodes_)
	{
		return nullptr;
	}

	return &nodes_[current_node_];
}

void AABB_tree_builder::split(Temp_aabb_node* node, std::vector<Static_prop*>& props)
{
	node->aabb = calculate_AABB(props);

    if (props.size() <= 64)
	{
		props.swap(node->props);

		return;
	}

	node->child0 = new Temp_aabb_node;
	node->child1 = new Temp_aabb_node;

	const float3& halfsize = node->aabb.halfsize;

	float3 min = node->aabb.get_min();
	float3 max = node->aabb.get_max();

	AABB aabb0;
	AABB aabb1;

	if (halfsize.x >= halfsize.y && halfsize.x >= halfsize.z)
	{
		aabb0.set_min_max(min, float3(node->aabb.position.x, max.y, max.z));
		aabb1.set_min_max(float3(node->aabb.position.x, min.y, min.z), max);
	}
	else if (halfsize.y >= halfsize.x && halfsize.y >= halfsize.z)
	{
		aabb0.set_min_max(min, float3(max.x, node->aabb.position.y, max.z));
		aabb1.set_min_max(float3(min.x, node->aabb.position.y, min.z), max);
	}
	else
	{
		aabb0.set_min_max(min, float3(max.x, max.y, node->aabb.position.z));
		aabb1.set_min_max(float3(min.x, min.y, node->aabb.position.z), max);
	}

    std::vector<Static_prop*> props0;
    std::vector<Static_prop*> props1;

	for (size_t i = 0; i < props.size(); ++i)
	{
		if (aabb0.intersect(props[i]->aabb()) == Intersection_type::Inside)
		{
            props0.push_back(props[i]);
		}
		else if (aabb1.intersect(props[i]->aabb()) == Intersection_type::Inside)
		{
            props1.push_back(props[i]);
		}
		else 
		{
			node->props.push_back(props[i]);
		}
	}

	split(node->child0, props0);
	split(node->child1, props1);
}

AABB AABB_tree_builder::calculate_AABB(const std::vector<Static_prop*>& props)
{				
    if (props.empty())
    {
        return AABB(float3::identity, float3::identity);
    }

	float3 min( 1000000.f,  1000000.f,  1000000.f);
	float3 max(-1000000.f, -1000000.f, -1000000.f);

	for (size_t i = 0; i < props.size(); ++i)
	{
		float3 tmin = props[i]->aabb().get_min();
		float3 tmax = props[i]->aabb().get_max();

		min = math::min(min, tmin);
		max = math::max(max, tmax);
	}

	return AABB(min, max);
}

AABB_tree_builder::Temp_aabb_node::Temp_aabb_node() : child0(nullptr), child1(nullptr)
{}

AABB_tree_builder::Temp_aabb_node::~Temp_aabb_node()
{
	delete child0;
	delete child1;
}

void AABB_tree_builder::Temp_aabb_node::get_num_sub_nodes(uint32_t& num_sub_nodes) const
{
	if (child0)
	{
		num_sub_nodes += 2;

		child0->get_num_sub_nodes(num_sub_nodes);
		child1->get_num_sub_nodes(num_sub_nodes);
	}
}

}
