#include "Clipping_job.hpp"
#include "Clipping.hpp"
#include "Math/Plane.hpp"
#include <iostream>

Clipping_job::Clipping_job()
{}

Clipping_job::~Clipping_job()
{}

void Clipping_job::clip(std::vector<Triangle>& destination, const std::vector<Triangle>& source, const Plane& splitting_plane)
{
	/*
	m_splittingPlane = &splittingPlane;
	m_source = &source;

	const size_t numWorkers = m_worker_pool.process(this, true);


	//gather the results
	size_t resultSize = 0;
	for (size_t i = 0; i < numWorkers; ++i)
		resultSize += m_results[i].size();

	result.clear();
	result.reserve(resultSize);

	for (size_t i = 0; i < numWorkers; ++i)
		result.insert(result.end(), m_results[i].begin(), m_results[i].end());
		*/

//  std::cout << "clipped with " << numWorkers << " workers." << std::endl;


	std::vector<Triangle>& triangles = destination;//result_;

	triangles.clear();
	triangles.reserve(source.size());

	std::vector<Triangle> result;
	result.reserve(3);

	std::vector<const rendering::Vertex_position3x32_tex_coord2x32_normal3x32*> positive;
	positive.reserve(3);

	std::vector<const rendering::Vertex_position3x32_tex_coord2x32_normal3x32*> negative;
	 negative.reserve(3);

	for (size_t i = 0; i < source.size(); ++i)
	{
		const Triangle& tri = source[i];

		Plane oldPlane(tri.v[0].position, tri.v[1].position, tri.v[2].position);

		result.clear();
		positive.clear(); negative.clear();
		clipTriangle(result, tri, splitting_plane, positive, negative);

		for (size_t j = 0; j < result.size(); ++j)
		{
			Plane newPlane(result[j].v[0].position, result[j].v[1].position, result[j].v[2].position);

			if (dot(newPlane.get_normal(), oldPlane.get_normal()) > 0.f)
			{
				triangles.push_back(result[j]);
			}
			else
			{
				triangles.push_back(Triangle(result[j].v[2], result[j].v[1], result[j].v[0]));
			}

			triangles.back().material_id = tri.material_id;
		}
	}
}
