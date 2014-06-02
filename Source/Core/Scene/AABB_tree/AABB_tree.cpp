#include "AABB_tree.hpp"
#include "AABB_node.hpp"
#include "Scene/Static_prop.hpp"

namespace scene
{

AABB_tree::AABB_tree() :
	num_geometry_clusters_(0), geometry_clusters_(nullptr), geometry_clusters_size_(0),
	num_nodes_(0), nodes_(nullptr), nodes_size_(0)
{}

AABB_tree::~AABB_tree()
{
	delete [] geometry_clusters_;
	delete [] nodes_;
}

void AABB_tree::clear()
{
	props_.clear();

    for (uint32_t i = 0; i < num_nodes_; ++i)
    {
		nodes_[i].clear();
    }

	num_nodes_ = 0;

	for (uint32_t i = 0; i < num_geometry_clusters_; ++i)
	{
		geometry_clusters_[i].clear();
	}

	num_geometry_clusters_ = 0;
}

bool AABB_tree::is_empty() const
{
    return 0 == num_nodes_;
}

const AABB_node* AABB_tree::root() const
{
	return nodes_;
}

const std::vector<Static_prop*>& AABB_tree::static_props() const
{
	return props_;
}

AABB_node::Geometry* AABB_tree::allocate_geometry_clusters(uint32_t num_geometry_clusters)
{
	if (num_geometry_clusters > geometry_clusters_size_)
	{
		delete [] geometry_clusters_;
		geometry_clusters_ = new AABB_node::Geometry[num_geometry_clusters];

		geometry_clusters_size_ = num_geometry_clusters;
	}

	num_geometry_clusters_ = num_geometry_clusters;

	return geometry_clusters_;
}

AABB_node* AABB_tree::allocate_nodes(uint32_t num_nodes)
{
	if (num_nodes > nodes_size_)
    {
		delete [] nodes_;
		nodes_ = new AABB_node[num_nodes];

		nodes_size_ = num_nodes;
    }

    num_nodes_ = num_nodes;

	return nodes_;
}

void AABB_tree::add_static_prop(Static_prop* prop)
{
	if (!add_static_prop(prop, nodes_))
	{
		props_.push_back(prop);
	}
}

bool AABB_tree::add_static_prop(Static_prop* prop, AABB_node* node)
{
	const AABB& aabb = prop->aabb();

	AABB_node* enclosing_node = nullptr;

	while (node)
	{
		Intersection_type intersection = node->aabb().intersect(aabb);

		if (Intersection_type::Inside == intersection)
		{
			enclosing_node = node;

			if (!node->has_children())
			{
				break;
			}
			else
			{
				++node;

				continue;
			}
		}
		else if (Intersection_type::Outside == intersection)
		{
			node = node->skip_node();
		}
		else
		{
			break;
		}
	}	

	if (enclosing_node)
	{
		enclosing_node->props_.push_back(prop);
		return true;
	}
	else
	{
		return false;
	}
}

}
