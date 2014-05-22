#include "AABB_tree_builder.hpp"
#include "Model/Model.hpp"
#include "AABB_tree.hpp"
#include "AABB_node.hpp"
#include <iostream>

AABB_tree_builder::AABB_tree_builder(uint32_t max_triangles_per_node) :
	m_calculateAabb(), m_clipping(), m_evaluateSplittingPlane(), m_assignNodeData(max_triangles_per_node),
	max_triangles_per_node_(max_triangles_per_node)
{}

AABB_tree* AABB_tree_builder::build(const Model& model)
{
	m_progress = 0.f;
	m_numUniqueVertices = 0;
	m_numTriangles = 0;
	num_geometry_clusters_ = 0;

	m_current_node_index = 0;

	std::vector<Triangle> triangles;

	triangles.reserve(model.indices.size() / 3);

	for (size_t g = 0; g < model.groups.size(); ++g)
	{
		const Model::Group& group = model.groups[g];

		for (size_t i = group.start_index; i < group.start_index + group.num_indices;)
		{
			Triangle tri;

			for (size_t v = 0; v < 3; ++v, ++i)
			{
				uint32_t index = model.indices[i];

				tri.v[v].position  = model.positions[index];
				tri.v[v].tex_coord  = model.texture_coordinates[index];
				tri.v[v].normal = model.normals[index];
			}

			tri.material_id = group.material_index;

			triangles.push_back(tri);
		}
	}

	m_numTargetTriangles = m_numOriginalTriangles = triangles.size();

	//---

	AABB_tree* tree = new AABB_tree;
	tree->root = new AABB_node;

	std::cout << "Progress..." << std::endl;
	split(tree->root, triangles);

	m_numUniqueVertices = m_assignNodeData.get_num_total_vertices();

	tree->num_geometry_clusters = num_geometry_clusters_;

	return tree;
}

size_t AABB_tree_builder::get_num_vertices() const
{
	return m_numUniqueVertices;
}

size_t AABB_tree_builder::get_num_triangles() const
{
	return m_numTriangles;
}

size_t AABB_tree_builder::get_num_original_triangles() const
{
	return m_numOriginalTriangles;
}

uint32_t AABB_tree_builder::get_num_geometry_clusters() const
{
	return num_geometry_clusters_;
}

uint32_t AABB_tree_builder::get_new_node_index()
{
	return m_current_node_index++;
}

void AABB_tree_builder::split(AABB_node *node, std::vector<Triangle>& triangles)
{
	node->aabb = m_calculateAabb.calculate_AABB(triangles);
	node->index = get_new_node_index();

	if (triangles.size() <= max_triangles_per_node_)
	{
		assign_data(node, triangles);
	}
	else
	{
		Plane splittingPlane = choose_splitting_plane(node->aabb, triangles);

		std::vector<Triangle> result;
		m_clipping.clip(result, triangles, splittingPlane);

		node->child0 = new AABB_node;
		split(node->child0, result);

		m_clipping.clip(result, triangles, -splittingPlane);

		// Only way I now of to reduce the capacity of an vector
		triangles.clear();
		std::vector<Triangle>(triangles).swap(triangles);

		node->child1 = new AABB_node;
		split(node->child1, result);
	}

	node->skip_offset = m_current_node_index - node->index;
}


bool AABB_tree_builder::assign_data(AABB_node *node, std::vector<Triangle> &triangles)
{
	m_assignNodeData.assign_data(node, triangles);

	m_numTriangles += triangles.size();

	node->geometry_index = num_geometry_clusters_;

	++num_geometry_clusters_;

	m_progress += float(triangles.size()) / float(m_numTargetTriangles);
	std::cout << math::round(m_progress * 870.f) / 10.f << " %" << std::endl;

	return true;
}

struct SplittingPlaneProperties
{
	Plane plane;
	size_t num_clips;

	SplittingPlaneProperties(const Plane& plane, size_t numClips) : plane(plane), num_clips(numClips)
	{}
};

bool planeSort(const SplittingPlaneProperties &a, const SplittingPlaneProperties &b)
{
	return a.num_clips < b.num_clips;
}

Plane AABB_tree_builder::choose_splitting_plane(const AABB& aabb, const std::vector<Triangle> &triangles)
{
	Plane plane;
	float width;

	if (aabb.halfsize.x >= aabb.halfsize.y && aabb.halfsize.x >= aabb.halfsize.z)
	{
		plane = Plane(float3(1.f, 0.f, 0.f), aabb.position);
		width = aabb.halfsize.x;
	}
	else if (aabb.halfsize.y >= aabb.halfsize.x && aabb.halfsize.y >= aabb.halfsize.z)
	{
		plane = Plane(float3(0.f, 1.f, 0.f), aabb.position);
		width = aabb.halfsize.y;
	}
	else
	{
		plane = Plane(float3(0.f, 0.f, 1.f), aabb.position);
		width = aabb.halfsize.z;
	}

	const float maxRelativeStep = 0.25f;
	const size_t numIterations = 16;

	if (!numIterations)
	{
		return plane;
	}

	const float step = maxRelativeStep / float(numIterations) * width;

	SplittingPlaneProperties bestSpp(plane, m_evaluateSplittingPlane.evaluate(plane, triangles));

	Plane posStep = plane;
	Plane negStep = plane;
	for (size_t i = 0; i < numIterations; ++i)
	{
		posStep.d += step;
		SplittingPlaneProperties posSpp(posStep, m_evaluateSplittingPlane.evaluate(posStep, triangles));

		negStep.d -= step;
		SplittingPlaneProperties negSpp(negStep, m_evaluateSplittingPlane.evaluate(negStep, triangles));

		if (posSpp.num_clips < bestSpp.num_clips)
		{
			bestSpp = posSpp;
		}

		if (negSpp.num_clips < bestSpp.num_clips)
		{
			bestSpp = negSpp;
		}
	}

	return bestSpp.plane;
}
