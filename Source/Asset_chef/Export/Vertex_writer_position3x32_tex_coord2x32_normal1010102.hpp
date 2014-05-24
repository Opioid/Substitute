#pragma once


#include "Vertex_writer.hpp"


class Vertex_writer_position3x32_tex_coord2x32_normal1010102 : public Vertex_writer
{

public:

	bool write_vertices(std::ostream &stream, const Model&  model) const;

	const rendering::Vertex_layout_description* vertex_layout_description() const;

};
