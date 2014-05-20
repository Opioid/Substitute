#pragma once

#include "Resources/Handle.hpp"
#include "AABB_node.hpp"
#include <string>
#include <vector>

namespace rendering
{

class Rendering_tool;
class Vertex_layout_description;

}

namespace scene
{

class AABB_tree;
class AABB_node;
class Material;

class AABB_tree_loader
{

public:

	AABB_tree_loader(rendering::Rendering_tool& rendering_tool);

	bool load(std::istream& stream, AABB_tree& tree, const std::vector<Handle<Material>>& materials);

private:

	struct Import_group
	{
		uint32_t m_material_id;

		uint32_t m_start_index;
		uint32_t num_indices_;
	};

	void read_geometry_clusters(uint32_t num_geometry_clusters, AABB_node::Geometry* geometry_clusters, const std::vector<Handle<Material>>& materials, std::istream& stream) const;

	void read(AABB_node& node, std::istream& stream) const;

	AABB_node::Geometry* get_geometry(uint32_t index) const;

	const rendering::Vertex_layout_description* read_vertex_layout_description(std::istream& stream) const;

	rendering::Rendering_tool& rendering_tool_;

	const rendering::Vertex_layout_description* m_vd;

	AABB_node::Geometry* geometry_clusters_;
};

}
