#pragma once

#include "Triangle.hpp"
#include "Calculate_AABB_job.hpp"
#include "Clipping_job.hpp"
#include "Evaluate_splitting_plane_job.hpp"
#include "Assign_node_data_job.hpp"
#include "Math/Intersection/AABB.hpp"
#include "Math/Plane.hpp"
#include <vector>

//namespace threading { class Worker_pool; }

struct Model;
struct AABB_tree;
struct AABB_node;

class AABB_tree_builder
{

public:

	AABB_tree_builder(uint32_t max_triangles_per_node/*, threading::Worker_pool &worker_pool*/);

	AABB_tree* build(const Model& model);

	size_t get_num_vertices() const;
	size_t get_num_triangles() const;
	size_t get_num_original_triangles() const;
	uint32_t get_num_geometry_clusters() const;

private:

	uint32_t get_new_node_index();

	void split(AABB_node* node, std::vector<Triangle>& triangles);
	bool assign_data(AABB_node* node, std::vector<Triangle>& triangles);

	Plane choose_splitting_plane(const AABB& aabb, const std::vector<Triangle>& triangles);

//	threading::Worker_pool&   m_worker_pool;
	Calculate_AABB_job          m_calculateAabb;
	Clipping_job               m_clipping;
	Evaluate_splitting_plane_job m_evaluateSplittingPlane;
	Assign_node_data_job         m_assignNodeData;

	const uint32_t m_max_triangles_per_node;

	uint32_t m_current_node_index;

	size_t m_numUniqueVertices;
	size_t m_numTriangles;
	size_t m_numOriginalTriangles;
	size_t m_numTargetTriangles;
	uint32_t num_geometry_clusters_;

	float m_progress;

};
