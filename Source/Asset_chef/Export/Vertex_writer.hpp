#pragma once

#include <fstream>

struct Model;

namespace rendering { class Vertex_layout_description; }

class Vertex_writer
{

public:

	bool write(std::ostream &stream, const Model&  model) const;

	static bool write_vertex_layout_description(std::ostream &stream, const rendering::Vertex_layout_description* vertex_layout_description);

private:

	virtual bool write_vertices(std::ostream &stream, const Model& model) const = 0;

	virtual const rendering::Vertex_layout_description* get_vertex_layout_description() const = 0;
};
