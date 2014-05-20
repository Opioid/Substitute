#pragma once

#include "Rendering/Vertex_layout_description.hpp"
#include <string>

class Legacy_converter
{

public:

	bool convert(const std::string& input_name, const std::string& output_name) const;

private:

	static void read_vertex_layout_description_element(std::istream& stream, rendering::Vertex_layout_description::Element& element);

	static bool write_vertex_layout_description(std::ostream &stream, const rendering::Vertex_layout_description* vertex_layout_description);
};
