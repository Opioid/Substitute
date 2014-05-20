#include "Exporter.hpp"
#include "Vertex_writer.hpp"
#include "Model/Model.hpp"
#include "Rendering/Data_format.hpp"

Exporter::Exporter() : m_vertex_writer(nullptr)
{}

void Exporter::set_vertex_writer(const Vertex_writer* vertex_writer)
{
	m_vertex_writer = vertex_writer;
}

bool Exporter::write(const std::string& name, const Model& model) const
{
	std::ofstream stream(name, std::ios::binary);

	if (!stream || !m_vertex_writer)
	{
		return false;
	}

	// Header
	const char header[] = "SUM\4";
	stream.write(header, sizeof(char) * 4);

	// Groups
	write_groups(stream, model);

	// Primitive Topology
	uint32_t pt = uint32_t(model.primitive_topology);
	stream.write((char*)&pt, sizeof(uint32_t));

	// Vertices
	if (!m_vertex_writer->write(stream, model))
	{
		return false;
	}

	// Indices
	rendering::Data_format::Value index_type = model.positions.size() < 65536 ? rendering::Data_format::R16_UInt : rendering::Data_format::R32_UInt;
	stream.write((char*)&index_type, sizeof(uint32_t));

	uint32_t num_indices = uint32_t(model.indices.size());
	stream.write((char*)&num_indices, sizeof(uint32_t));

	if (rendering::Data_format::R16_UInt == index_type)
	{
		for (size_t i = 0; i < model.indices.size(); ++i)
		{
			uint16_t index = uint16_t(model.indices[i]);
			stream.write((char*)&index, sizeof(uint16_t));
		}
	}
	else if (rendering::Data_format::R32_UInt == index_type)
	{
		stream.write((char*)&model.indices[0], sizeof(uint32_t) * model.indices.size());
	}

	// AABB
	AABB aabb = model.calculate_aabb();
	stream.write((char*)&aabb, sizeof(AABB));

	// Skinned
	bool skinned = false;
	stream.write((char*)&skinned, sizeof(bool));

	return true;
}

void Exporter::write_groups(std::ostream &stream, const Model& model) const
{
	std::vector<Model::Group> groups;

	if (!model.groups.empty())
	{
		groups.push_back(model.groups[0]);

		for (size_t i = 1; i < model.groups.size(); ++i)
		{
			Model::Group& previous_group = groups.back();

			if (model.groups[i].material_index == previous_group.material_index
			&&  model.groups[i].start_index == previous_group.start_index + previous_group.num_indices)
			{
				previous_group.num_indices += model.groups[i].num_indices;
			}
			else
			{
				groups.push_back(model.groups[i]);
			}
		}
	}

	// #Groups
	uint32_t num_groups = uint32_t(groups.size());
	stream.write((char*)&num_groups, sizeof(uint32_t));

	// Array of groups
	stream.write((char*)&groups[0], sizeof(Model::Group) * groups.size());
}
