#include "Assign_node_data_job.hpp"
#include "AABB_node.hpp"
#include <algorithm>
#include <iostream>


Assign_node_data_job::Assign_node_data_job(size_t maxTrianglesPerNode) : num_total_vertices_(0)
{
	m_workData.triangles.reserve(maxTrianglesPerNode);
	m_workData.vertexPool.init(3 * maxTrianglesPerNode);
}

Assign_node_data_job::~Assign_node_data_job()
{
}

bool triangleSort(const Triangle &a, const Triangle &b)
{
	return a.material_id > b.material_id;
}

void Assign_node_data_job::assign_data(AABB_node *node, std::vector<Triangle> &triangles)
{
	VertexPool &vertexPool = m_workData.vertexPool;
	vertexPool.clear();

	sort(triangles.begin(), triangles.end(), triangleSort);

	node->geometry = new AABB_node::Geometry;

	std::vector<AABB_node::Geometry::Group> groups;

	uint32_t materialId = triangles[0].material_id;
	uint32_t startIndex = 0;
	uint32_t num_indices = 0;

	for (size_t i = 0; i < triangles.size(); ++i, num_indices += 3)
	{
		if (triangles[i].material_id != materialId)
		{
			AABB_node::Geometry::Group group;

			group.material_id = materialId;
			group.start_index = startIndex;
			group.num_indices = num_indices;

			groups.push_back(group);

			materialId = triangles[i].material_id;
			startIndex += num_indices;
			num_indices = 0;
		}
	}

	AABB_node::Geometry::Group group;

	group.material_id = materialId;
	group.start_index = startIndex;
	group.num_indices = num_indices;

	groups.push_back(group);

	node->geometry->num_groups = groups.size();
	node->geometry->groups = new AABB_node::Geometry::Group[groups.size()];

	for (size_t i = 0; i < groups.size(); ++i)
	{
		node->geometry->groups[i] = groups[i];
	}

	node->geometry->num_indices = triangles.size() * 3;
	node->geometry->indices = new unsigned short[node->geometry->num_indices];

	for (size_t i = 0, j = 0; i < triangles.size(); ++i)
	{
		node->geometry->indices[j++] = static_cast<unsigned short>(vertexPool.getIndex(triangles[i].v[0]));
		node->geometry->indices[j++] = static_cast<unsigned short>(vertexPool.getIndex(triangles[i].v[1]));
		node->geometry->indices[j++] = static_cast<unsigned short>(vertexPool.getIndex(triangles[i].v[2]));
	}

	node->geometry->num_vertices = vertexPool.getNumVertices();
/*	node->geometry_->vertices_ = new Vertex_Pos3x32_Tex2x32_Norm1010102_Tan1010102[node->geometry_->m_numVertices];

	const size_t vertexBytes = sizeof(Vertex_Pos3x32_Tex2x32_Norm1010102_Tan1010102) * node->geometry_->m_numVertices;
	memcpy(node->geometry_->vertices_, vertexPool.getVertices(), vertexBytes);
	*/

	node->geometry->vertices = new rendering::Vertex_position3x32_tex_coord2x32_normal1010102[node->geometry->num_vertices];

	auto vertices = vertexPool.getVertices();

	for (uint32_t i = 0; i < node->geometry->num_vertices; ++i)
	{
		node->geometry->vertices[i].position = vertices[i].position;
		node->geometry->vertices[i].tex_coord = vertices[i].tex_coord;
		node->geometry->vertices[i].normal = vertices[i].normal;
	}

	num_total_vertices_ += vertexPool.getNumVertices();
}

size_t Assign_node_data_job::get_num_total_vertices() const
{
	return num_total_vertices_;
}

Assign_node_data_job::VertexPool::VertexPool(size_t maxVertices) : m_maxVertices(maxVertices), vertices_(new rendering::Vertex_position3x32_tex_coord2x32_normal1010102[maxVertices]), m_numVertices(0)
{}

Assign_node_data_job::VertexPool::~VertexPool()
{
	delete [] vertices_;
}


void Assign_node_data_job::VertexPool::init(size_t maxVertices)
{
	m_maxVertices = maxVertices;
	delete [] vertices_;
	vertices_ = new rendering::Vertex_position3x32_tex_coord2x32_normal1010102[maxVertices];
	m_numVertices = 0;
}


void Assign_node_data_job::VertexPool::clear()
{
	m_numVertices = 0;
}


size_t Assign_node_data_job::VertexPool::getIndex(const rendering::Vertex_position3x32_tex_coord2x32_normal3x32 &vertex)
{
	rendering::Vertex_position3x32_tex_coord2x32_normal1010102 v;

	v.position  = vertex.position;
	v.tex_coord  = vertex.tex_coord;
	v.normal = Vector4_UNorm1010102(vertex.normal, 1.f);

	for (size_t i = 0; i < m_numVertices; ++i)
	{
		if (more_or_less_equal(vertices_[i], v))
		{
			return i;
		}
	}

	vertices_[m_numVertices] = v;

	return m_numVertices++;
}


const rendering::Vertex_position3x32_tex_coord2x32_normal1010102 *Assign_node_data_job::VertexPool::getVertices() const
{
	return vertices_;
}


size_t Assign_node_data_job::VertexPool::getNumVertices() const
{
	return m_numVertices;
}


bool Assign_node_data_job::VertexPool::more_or_less_equal(const float2 &a, const float2 &b)
{
	static const float e = 0.00001f;

	return std::abs(a.x - b.x) < e && std::abs(a.y - b.y) < e;
}


bool Assign_node_data_job::VertexPool::more_or_less_equal(const float3 &a, const float3 &b)
{
	static const float e = 0.00001f;

	return std::abs(a.x - b.x) < e && std::abs(a.y - b.y) < e && std::abs(a.z - b.z) < e;
}


bool Assign_node_data_job::VertexPool::more_or_less_equal(const rendering::Vertex_position3x32_tex_coord2x32_normal1010102 &a, const rendering::Vertex_position3x32_tex_coord2x32_normal1010102 &b)
{
	return a.normal == b.normal
		&& more_or_less_equal(a.tex_coord, b.tex_coord)
		&& more_or_less_equal(a.position, b.position);
}
