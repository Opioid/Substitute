#pragma once

#include "Math/Intersection/AABB.hpp"
#include "../Renderable.hpp"
#include <cstdint>
#include <vector>

namespace scene
{

class Static_prop;
class AABB_tree;
class AABB_tree_builder;
class AABB_tree_loader;

class AABB_node
{

public:
	
	class Geometry : public Renderable
	{

	public:

		Geometry();
		~Geometry();

		struct Group
		{
			uint32_t material_index;
 
			uint32_t start_index;
			uint32_t num_indices;
		};

		bool create_surfaces(uint32_t num_groups, Group* groups, const rendering::Vertex_layout_description* vd, const std::vector<Handle<Material>>& materials);

		void clear();

		Handle<rendering::Vertex_buffer>* vertex_buffers_;
		Handle<rendering::Index_buffer>   index_buffer_;

		uint32_t num_vertices_;
		uint32_t num_indices_;
	};

	AABB_node();
	~AABB_node();

    void clear();

	bool has_children() const;

    const AABB_node* get_skip_node() const;
	AABB_node* get_skip_node();

	const AABB& aabb() const;

	const Geometry* get_geometry() const;

	const std::vector<Static_prop*>& get_static_props() const;

private:

	void set_has_children(bool children);
	void set_skip_node(AABB_node* node);

	AABB aabb_;

	uint32_t skip_offset_;

	Geometry* geometry_;

	std::vector<Static_prop*> props_;

	static const uint32_t s_has_children = 0x80000000;

	friend AABB_tree;
	friend AABB_tree_builder;
	friend AABB_tree_loader;
};

}
