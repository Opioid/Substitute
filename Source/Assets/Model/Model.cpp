#include "Model.hpp"

Model::Model() : primitive_topology(rendering::Primitive_topology::Triangle_list)
{}

bool Model::has_positions() const
{
	return positions.size() > 0;
}

bool Model::has_texture_coordinates() const
{
	return texture_coordinates.size() > 0;
}

bool Model::has_normals() const
{
	return normals.size() > 0;
}

bool Model::has_tangent_space() const
{
	return tangents_and_bitangent_signs.size() > 0;
}

AABB Model::calculate_aabb() const
{
	float3 mini( 1000000.f,  1000000.f,  1000000.f);
	float3 maxi(-1000000.f, -1000000.f, -1000000.f);

	for (size_t i = 0; i < positions.size(); ++i)
	{
		mini = math::min(positions[i], mini);
		maxi = math::max(positions[i], maxi);
	}

	return AABB(mini, maxi);
}

void Model::scale(float s)
{
	float3 sv(s, s, s);

	for (size_t i = 0; i < positions.size(); ++i)
	{
		positions[i] *= sv;
	}
}

void Model::scale(const float3& s)
{
	for (size_t i = 0; i < positions.size(); ++i)
	{
		positions[i] *= s;
	}
}

void Model::rotate(const float3x3& m)
{
	for (size_t i = 0; i < positions.size(); ++i)
	{
		positions[i] *= m;
	}

	for (size_t i = 0; i < normals.size(); ++i)
	{
		normals[i] = normalize(normals[i] * m);
	}

	for (size_t i = 0; i < tangents_and_bitangent_signs.size(); ++i)
	{
		tangents_and_bitangent_signs[i] = float4(tangents_and_bitangent_signs[i].xyz * m, tangents_and_bitangent_signs[i].w);
	}
}

void Model::set_origin_center_bottom()
{
	AABB aabb = calculate_aabb();

	for (size_t i = 0; i < positions.size(); ++i)
	{
		positions[i].y -= aabb.halfsize.y;
	//	positions[i].x -= aabb.position.x;
	//	positions[i].z -= aabb.position.z;
	}
}