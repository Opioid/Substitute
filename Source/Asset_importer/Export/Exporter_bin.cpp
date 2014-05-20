#include "Exporter_bin.hpp"
#include "Model/Model.hpp"
#include <fstream>

bool Exporter_bin::write(const std::string& name, const Model&  model) const
{
	std::ofstream stream(name + ".bin", std::ios::binary);

	if (!stream)
	{
		return false;
	}

	static char const header[] = "BIN";
	stream.write(header, sizeof(char) * 3);

	// #Groups
	uint32_t num_groups = uint32_t(model.groups.size());
	stream.write((char*)&num_groups, sizeof(uint32_t));

	// Array of groups
	stream.write((char*)&model.groups[0], sizeof(Model::Group) * model.groups.size());

	// Primitive Topology
	uint32_t pt = uint32_t(model.primitive_topology);
	stream.write((char*)&pt, sizeof(uint32_t));

	// #Positions
	uint32_t num_positions = uint32_t(model.positions.size());
	stream.write((char*)&num_positions, sizeof(uint32_t));

	// Array of positions
	stream.write((char*)&model.positions[0], sizeof(float3) * model.positions.size());

	// #Texture coordinates
	uint32_t num_coordinates = uint32_t(model.texture_coordinates.size());
	stream.write((char*)&num_coordinates, sizeof(uint32_t));

	// Array of texture coordinates
	stream.write((char*)&model.texture_coordinates[0], sizeof(float2) * model.texture_coordinates.size());

	// #Normals
	uint32_t num_normals = uint32_t(model.normals.size());
	stream.write((char*)&num_normals, sizeof(uint32_t));

	// Array of normals
	stream.write((char*)&model.normals[0], sizeof(float3) * model.normals.size());

	// #Tangents
	uint32_t num_tangents = uint32_t(model.tangents_and_bitangent_signs.size());
	stream.write((char*)&num_tangents, sizeof(uint32_t));

	//Array of tangents
	stream.write((char*)&model.tangents_and_bitangent_signs[0], sizeof(float4) * model.tangents_and_bitangent_signs.size());

	// #Indices
	uint32_t num_indices = uint32_t(model.indices.size());
	stream.write((char*)&num_indices, sizeof(uint32_t));

	// Array of indices
	stream.write((char*)&model.indices[0], sizeof(uint32_t) * model.indices.size());

	return true;
}
