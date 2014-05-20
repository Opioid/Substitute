#pragma once

#include "Triangle.hpp"
#include <vector>

struct AABB_node;

class Assign_node_data_job
{

public:

	Assign_node_data_job(size_t maxTrianglesPerNode);
	~Assign_node_data_job();

	void assign_data(AABB_node* node, std::vector<Triangle>& triangles);

	size_t get_num_total_vertices() const;

private:

	size_t num_total_vertices_;

	class VertexPool
	{

	public:

		VertexPool(size_t maxVertices = 0);
		~VertexPool();

		void init(size_t maxVertices);
		void clear();

		size_t getIndex(const rendering::Vertex_position3x32_tex_coord2x32_normal3x32& vertex);

		const rendering::Vertex_position3x32_tex_coord2x32_normal1010102 *getVertices() const;
		size_t getNumVertices() const;

	private:

		rendering::Vertex_position3x32_tex_coord2x32_normal1010102 *vertices_;
		size_t m_maxVertices;
		size_t m_numVertices;

		static bool more_or_less_equal(const float2& a, const float2& b);
		static bool more_or_less_equal(const float3& a, const float3& b);
		static bool more_or_less_equal(const rendering::Vertex_position3x32_tex_coord2x32_normal1010102 &a, const rendering::Vertex_position3x32_tex_coord2x32_normal1010102 &b);
	};


	struct WorkData
	{
		VertexPool vertexPool;
		AABB_node *node;
		std::vector<Triangle> triangles;
	};


	WorkData m_workData;


	struct WorkArgument
	{
		WorkArgument(AABB_node *node, const std::vector<Triangle> &triangles) : node(node), triangles(triangles)
		{}

		AABB_node *node;
		const std::vector<Triangle> &triangles;
	};
};
