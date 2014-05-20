#include "AABB_tree_writer.hpp"
#include "AABB_tree.hpp"
#include "Rendering/Primitive_topology.hpp"
#include "../Export/Vertex_writer.hpp"

bool AABB_tree_writer::write(const std::string& name, const AABB_tree& tree)
{
	std::ofstream stream(name, std::ios::binary);

	if (!stream)
	{
		return false;
	}

	//Header
	static char const header[] = "ABT\2";
	stream.write(header, sizeof(char) * 4);

	//Primitive Topology
	uint32_t pt = uint32_t(rendering::Primitive_topology::Triangle_list);
	stream.write((char*)&pt, sizeof(uint32_t));

	//Vertex layout description
	Vertex_writer::write_vertex_layout_description(stream, rendering::Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description());

	//Geometry clusters
	write_geometry_clusters(tree, stream);

	//#Nodes
	num_nodes_ = 1;
	tree.root->get_num_sub_nodes(num_nodes_);

	stream.write((char*)&num_nodes_, sizeof(uint32_t));

	write(tree.root, stream);

	return true;
}


void AABB_tree_writer::write_geometry_clusters(const AABB_tree& tree, std::ofstream &stream)
{
	//#Geometry clusters
	stream.write((char*)&tree.num_geometry_clusters, sizeof(uint32_t));

	write_geometry_cluster(tree.root, stream);
}


void AABB_tree_writer::write_geometry_cluster(AABB_node* node, std::ofstream &stream)
{
	if (node->geometry)
	{
		const AABB_node::Geometry& geometry = *node->geometry;

		// Groups
		stream.write((char*)&geometry.num_groups, sizeof(uint32_t));
		stream.write((char*)geometry.groups, sizeof(AABB_node::Geometry::Group) * geometry.num_groups);

		// Vertices
		stream.write((char*)&geometry.num_vertices, sizeof(uint32_t));

		/*
		uint32_t vertexStride = sizeof(float3);
		stream.write((char*)geometry.m_positions, vertexStride * geometry.m_numVertices);

		vertexStride = sizeof(float2);
		stream.write((char*)geometry.m_texCoords, vertexStride * geometry.m_numVertices);

		vertexStride = 2 * sizeof(Vector4_UNorm1010102);
		stream.write((char*)geometry.m_normalsAndTangents, vertexStride * geometry.m_numVertices);
		*/
		stream.write((char*)geometry.vertices, sizeof(rendering::Vertex_position3x32_tex_coord2x32_normal1010102) * geometry.num_vertices);

		// Indices
		stream.write((char*)&geometry.num_indices, sizeof(uint32_t));
		stream.write((char*)geometry.indices, sizeof(unsigned short) * geometry.num_indices);
	}
	else
	{
		write_geometry_cluster(node->child0, stream);
		write_geometry_cluster(node->child1, stream);
	}
}


void AABB_tree_writer::write(AABB_node* node, std::ofstream& stream)
{
	// AABB
	stream.write((char*)&node->aabb, sizeof(AABB));
		
	uint32_t skip_offset = node->index + node->skip_offset >= num_nodes_ ? 0 : node->skip_offset;
	stream.write((char*)&skip_offset, sizeof(uint32_t));

	stream.write((char*)&node->geometry_index, sizeof(uint32_t));

	// Children
	if (!node->geometry)
	{
		write(node->child0, stream);
		write(node->child1, stream);
	}
}
