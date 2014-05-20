#pragma once

#include <string>
#include <fstream>

class Vertex_writer;
struct Model;

class Exporter
{
public:

	Exporter();

	void set_vertex_writer(const Vertex_writer* vertex_writer);

	bool write(const std::string& name, const Model& model) const;

private:

	void write_groups(std::ostream &stream, const Model& model) const;

	const Vertex_writer* m_vertex_writer;
};
