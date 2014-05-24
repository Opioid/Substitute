#include "AABB_tree_loader.hpp"
#include "AABB_tree.hpp"
#include "AABB_node.hpp"
#include "../../Rendering/Rendering_tool.hpp"
#include "../Material.hpp"
#include <cstring>

namespace scene
{

AABB_tree_loader::AABB_tree_loader(rendering::Rendering_tool& rendering_tool) : rendering_tool_(rendering_tool)
{}

bool AABB_tree_loader::load(std::istream& stream, AABB_tree& tree, const std::vector<Handle<Material>>& materials)
{
	char header[3];
	stream.read(header, sizeof(char) * 3);

	if (strncmp("ABT", header, 3)) 
	{
		return false;
	}

	unsigned char version = 0;
	stream.read((char*)&version, sizeof(unsigned char));

	if (2 != version) 
	{
		return false;
	}

	// Primitve toplogy
	rendering::Primitive_topology pt;
	stream.read((char*)&pt, sizeof(uint32_t));

	// Vertex description
	m_vd = read_vertex_layout_description(stream);

	// #Geometry clusters
	uint32_t num_geometry_clusters;
	stream.read((char*)&num_geometry_clusters, sizeof(uint32_t));

	geometry_clusters_ = tree.allocate_geometry_clusters(num_geometry_clusters);

	read_geometry_clusters(num_geometry_clusters, geometry_clusters_, materials, stream);

	// #Nodes
	uint32_t num_nodes;
	stream.read((char*)&num_nodes, sizeof(uint32_t));

    AABB_node* nodes = tree.allocate_nodes(num_nodes);

	for (uint32_t i = 0; i < num_nodes; ++i)
	{
		read(nodes[i], stream);
	}

    return true;
}

void AABB_tree_loader::read_geometry_clusters(uint32_t num_geometry_clusters, AABB_node::Geometry* geometry_clusters, const std::vector<Handle<Material>>& materials, std::istream& stream) const
{
	for (uint32_t g = 0; g < num_geometry_clusters; ++g)
	{
		AABB_node::Geometry& geometry = geometry_clusters[g];

		uint32_t num_groups = 0;

		// Groups
		stream.read((char*)&num_groups, sizeof(uint32_t));

		AABB_node::Geometry::Group* groups = new AABB_node::Geometry::Group[num_groups];

		stream.read((char*)groups, sizeof(AABB_node::Geometry::Group) * num_groups);

		// Vertices
		stream.read((char*)&geometry.num_vertices_, sizeof(uint32_t));

		geometry.vertex_buffers_ = new Handle<rendering::Vertex_buffer>[m_vd->num_streams()];

		for (uint32_t i = 0; i < m_vd->num_streams(); ++i)
		{
			uint32_t num_vertex_bytes = m_vd->strides()[i] * geometry.num_vertices_;
			char *vertices = new char[num_vertex_bytes];
			stream.read(vertices, num_vertex_bytes);

			geometry.vertex_buffers_[i] = rendering_tool_.device().create_vertex_buffer(num_vertex_bytes, vertices);

			delete [] vertices;
		}

		// Indices--
		stream.read((char*)&geometry.num_indices_, sizeof(uint32_t));

		const uint32_t indexBytes = sizeof(unsigned short) * geometry.num_indices_;
		char *indices = new char[indexBytes];
		stream.read(indices, indexBytes);

		geometry.index_buffer_ = rendering_tool_.device().create_index_buffer(indexBytes, indices, rendering::Data_format::R16_UInt);
	
		delete [] indices;

		geometry.create_surfaces(num_groups, groups, m_vd, materials);

		delete [] groups;
	}

}

void AABB_tree_loader::read(AABB_node& node, std::istream& stream) const
{
	// AABB
	stream.read((char*)&node.aabb_, sizeof(AABB));

	// Skip offset
	stream.read((char*)&node.skip_offset_, sizeof(uint32_t));

	uint32_t geometry_index;
	stream.read((char*)&geometry_index, sizeof(uint32_t));

	node.geometry_ = get_geometry(geometry_index);

	node.set_has_children(nullptr == node.geometry_);
}

AABB_node::Geometry* AABB_tree_loader::get_geometry(uint32_t index) const
{
	if (0xFFFFFFFF == index)
	{
		return nullptr;
	}
	else
	{
		return &geometry_clusters_[index];
	}
}

const rendering::Vertex_layout_description* AABB_tree_loader::read_vertex_layout_description(std::istream& stream) const
{
	uint32_t num_elements_;
	stream.read((char*)&num_elements_, sizeof(uint32_t));

	rendering::Vertex_layout_description::Element* elements = new rendering::Vertex_layout_description::Element[num_elements_];

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		stream >> elements[i];
	}

	const rendering::Vertex_layout_description* description = rendering_tool_.vertex_layout_cache().vertex_layout_description(num_elements_, elements);
	delete [] elements;
	return description;
}

}
