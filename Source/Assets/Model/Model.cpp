#include "Model.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"

#include <iostream>

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
		float3 n = normalize(normals[i] * m);

		if (!std::isfinite(n.x) || !std::isfinite(n.y) || !std::isfinite(n.z)) {
			n = float3(0.f, 1.f, 0.f);
		}

		normals[i] = n;
	}

	for (size_t i = 0; i < tangents_and_bitangent_signs.size(); ++i)
	{
		tangents_and_bitangent_signs[i] = float4(tangents_and_bitangent_signs[i].xyz * m, tangents_and_bitangent_signs[i].w);
	}
}

void Model::set_origin_center()
{
	AABB aabb = calculate_aabb();

	// TODO implement this properly

	for (size_t i = 0; i < positions.size(); ++i)
	{
		positions[i].x -= aabb.position.x;
		positions[i].y -= aabb.position.y;
		positions[i].z -= aabb.position.z;
	}
}

void Model::set_origin_center_bottom()
{
	AABB aabb = calculate_aabb();

	float3 offset = float3(-aabb.position.x, aabb.halfsize.y - aabb.position.y, -aabb.position.z);

	for (size_t i = 0; i < positions.size(); ++i)
	{
		positions[i] += offset;
	}
}


static inline void orthonormal_basis(const float3& n, float3& t, float3& b) {
	// Building an Orthonormal Basis, Revisited
	// http://jcgt.org/published/0006/01/01/

	const float sign = std::copysign(1.f, n.z);
	const float c = -1.f / (sign + n.z);
	const float d = n.x * n.y * c;
	t = float3(1.f + sign * n.x * n.x * c, sign * d, -sign * n.x);
	b = float3(d, sign + n.y * n.y * c, -n.y);
}


void Model::fix_tangent_frame() {
	size_t num_fixed_normals = 0;

	for (size_t i = 0, len = normals.size(); i < len; ++i) {
		float3& n = normals[i];

		if (squared_length(n) < 0.1f) {
			n = float3(0.f, 1.f, 0.f);
			++num_fixed_normals;
		}
	}

	std::cout << "Fixed " << num_fixed_normals << " normals" << std::endl;

	size_t num_fixed_tangents = 0;

	for (size_t i = 0, len = tangents_and_bitangent_signs.size(); i < len; ++i) {
		float4& tbs = tangents_and_bitangent_signs[i];

		if (squared_length(tbs.xyz) < 0.1f) {
			const float3& n = normals[i];
			float3 t, b;
			orthonormal_basis(n, t, b);

			tbs = float4(t, 1.f);
			++num_fixed_tangents;
		}
	}

	std::cout << "Fixed " << num_fixed_tangents << " tangents" << std::endl;
}
