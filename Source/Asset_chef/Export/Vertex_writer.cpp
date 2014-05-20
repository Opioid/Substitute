#include "Vertex_writer.hpp"
#include "Model/Model.hpp"
#include "Rendering/Vertex_layout_description.hpp"
#include <cstdint>

bool Vertex_writer::write(std::ostream &stream, const Model&  model) const
{
	if (!write_vertex_layout_description(stream, get_vertex_layout_description()))
	{
		return false;
	}

	uint32_t num_vertices = uint32_t(model.positions.size());
	stream.write((char*)&num_vertices, sizeof(uint32_t));

	if (!write_vertices(stream, model))
	{
		return false;
	}

	return true;
}

bool Vertex_writer::write_vertex_layout_description(std::ostream &stream, const rendering::Vertex_layout_description* vertex_layout_description)
{
	if (!vertex_layout_description)
	{
		return false;
	}

	uint32_t num_elements = vertex_layout_description->get_num_elements();
	stream.write((char*)&num_elements, sizeof(uint32_t));

	for (uint32_t i = 0; i < num_elements; ++i)
	{
		rendering::Vertex_layout_description::Element const& e = (*vertex_layout_description)[i];

		uint32_t name_length = uint32_t(e.semantic_name.size());
		stream.write((char*)&name_length, sizeof(uint32_t));
		stream.write((char*)e.semantic_name.c_str(), sizeof(char) * name_length);

		stream.write((char*)&e.semantic_index, sizeof(uint32_t));
		stream.write((char*)&e.format, sizeof(uint32_t));
		stream.write((char*)&e.slot, sizeof(uint32_t));
		stream.write((char*)&e.byte_offset, sizeof(uint32_t));
		stream.write((char*)&e.slot_class, sizeof(uint32_t));
		stream.write((char*)&e.instance_step_rate, sizeof(uint32_t));
	}

	return true;
}
