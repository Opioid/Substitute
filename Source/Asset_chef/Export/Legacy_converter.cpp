#include "Legacy_converter.hpp"
#include "Model/Model.hpp"
#include "File/Stream.hpp"
#include "Rendering/Vertex_format.hpp"
#include <cstring>
#include <fstream>
#include <iostream>

bool Legacy_converter::convert(const std::string& input_name, const std::string& output_name) const
{
	std::ifstream istream(input_name, std::ios::binary);

	if (!istream)
	{
        std::cout << "Could not open \"" << input_name << "\"" << std::endl;
		return false;
	}

	// Header
	std::ofstream ostream(output_name, std::ios::binary);

	if (!ostream)
	{
		return false;
	}

	char iheader[3];
	istream.read(iheader, sizeof(char) * 3);

	if (strncmp("SUM", iheader, 3))
	{
		return nullptr;
	}

	unsigned char version = 0;
	istream.read((char*)&version, sizeof(unsigned char));

	if (4 != version)
	{
		return false;
	}

	static char const oheader[] = "SUM\4";
	ostream.write(oheader, sizeof(char) * 4);

	// Groups
	uint32_t num_groups;
	istream.read((char*)&num_groups, sizeof(uint32_t));

	Model::Group* groups = new Model::Group[num_groups];
	istream.read((char*)groups, sizeof(Model::Group) * num_groups);

	ostream.write((char*)&num_groups, sizeof(uint32_t));
	ostream.write((char*)groups, sizeof(Model::Group) * num_groups);

	// Primitive Topology
	uint32_t pt;
	istream.read((char*)&pt, sizeof(uint32_t));
	ostream.write((char*)&pt, sizeof(uint32_t));

	// Vertex description
	uint32_t num_elements_;
	istream.read((char*)&num_elements_, sizeof(uint32_t));

	rendering::Vertex_layout_description::Element* elements = new rendering::Vertex_layout_description::Element[num_elements_];

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		read_vertex_layout_description_element(istream, elements[i]);
	}

	rendering::Vertex_layout_description vd(num_elements_, elements);

	if (!write_vertex_layout_description(ostream, rendering::Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description()))
	{
		delete [] elements;

		return false;
	}

	// Vertices
	uint32_t num_vertices;
	istream.read((char*)&num_vertices, sizeof(uint32_t));
	ostream.write((char*)&num_vertices, sizeof(uint32_t));

	uint32_t num_streams = vd.get_num_streams();
	char** vertex_streams = new char*[num_streams];

	for (uint32_t i = 0; i < num_streams; ++i)
	{
		uint32_t vertex_bytes = vd.get_strides()[i] * num_vertices;
		vertex_streams[i] = new char[vertex_bytes];
		istream.read(vertex_streams[i], vertex_bytes);
	}

	uint32_t num_out_vertex_bytes = sizeof(rendering::Vertex_position3x32_tex_coord2x32_normal1010102) * num_vertices;
	char* out_vertices = new char[num_out_vertex_bytes];

	char* ovs = out_vertices;

	for (uint32_t i = 0; i < num_vertices; ++i)
	{
		memcpy(ovs, vertex_streams[0], vd.get_strides()[0]);
		ovs += vd.get_strides()[0];
		vertex_streams[0] += vd.get_strides()[0];

		memcpy(ovs, vertex_streams[1], vd.get_strides()[1]);
		ovs += vd.get_strides()[1];
		vertex_streams[1] += vd.get_strides()[1];

		memcpy(ovs, vertex_streams[2], sizeof(Vector4_UNorm1010102));
		ovs += sizeof(Vector4_UNorm1010102);
		vertex_streams[2] += vd.get_strides()[2];
	}

	ostream.write(out_vertices, num_out_vertex_bytes);

	delete [] elements;

	// Indices--
	rendering::Data_format::Value index_type;
	istream.read((char*)&index_type, sizeof(uint32_t));
	uint32_t num_indices;
	istream.read((char*)&num_indices, sizeof(uint32_t));

	ostream.write((char*)&index_type, sizeof(uint32_t));
	ostream.write((char*)&num_indices, sizeof(uint32_t));

	const uint32_t num_index_bytes = num_indices * uint32_t(rendering::Data_format::size_of(index_type));
	char* indices = new char[num_index_bytes];
	istream.read(indices, num_index_bytes);
	ostream.write(indices, num_index_bytes);

	// AABB
	AABB aabb;
	istream.read((char*)&aabb, sizeof(AABB));
	ostream.write((char*)&aabb, sizeof(AABB));

	bool skinned;
	istream.read((char*)&skinned, sizeof(bool));
	ostream.write((char*)&skinned, sizeof(bool));

	return true;
}

void Legacy_converter::read_vertex_layout_description_element(std::istream& stream, rendering::Vertex_layout_description::Element& element)
{
	element.semantic_name = file::read_string(stream);

	stream.read((char*)&element.semantic_index, sizeof(uint32_t));
	stream.read((char*)&element.format, sizeof(uint32_t));
	stream.read((char*)&element.slot, sizeof(uint32_t));
	stream.read((char*)&element.byte_offset, sizeof(uint32_t));
	stream.read((char*)&element.slot_class, sizeof(uint32_t));
	stream.read((char*)&element.instance_step_rate, sizeof(uint32_t));
}

bool Legacy_converter::write_vertex_layout_description(std::ostream &stream, const rendering::Vertex_layout_description* vertex_layout_description)
{
	if (!vertex_layout_description)
	{
		return false;
	}

	uint32_t num_elements_ = vertex_layout_description->get_num_elements_();
	stream.write((char*)&num_elements_, sizeof(uint32_t));

	for (uint32_t i = 0; i < num_elements_; ++i)
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

