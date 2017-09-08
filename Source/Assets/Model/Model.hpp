#pragma once

#include "Math/Vector.hpp"
#include "Math/Intersection/AABB.hpp"
#include "Rendering/Primitive_topology.hpp"
#include <map>
#include <vector>
#include <cstdint>

struct Model
{
	Model();

	bool has_positions() const;
	bool has_texture_coordinates() const;
	bool has_normals() const;
	bool has_tangent_space() const;

	AABB calculate_aabb() const;

	void scale(float s);
	void scale(const float3& s);

	void rotate(const float3x3& m);

	void set_origin_center();
	void set_origin_center_bottom();

	void fix_tangent_frame();

	struct Group
	{
		uint32_t material_index;
 
		uint32_t start_index;
		uint32_t num_indices;
	};

	std::vector<Group> groups;

	rendering::Primitive_topology primitive_topology;

	std::vector<float3> positions;
	std::vector<float2> texture_coordinates;
	std::vector<float3> normals;
	std::vector<float4> tangents_and_bitangent_signs;

	std::vector<uint32_t> indices;

	struct Material {
		std::string name;

		std::string diffuse_texture;
		std::string mask_texture;
		std::string normal_texture;
		std::string spec_texture;

		float roughness;
		float opacity;

		bool two_sided;
	};

	std::map<uint32_t, Material> materials;
};
