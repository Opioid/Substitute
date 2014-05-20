#include "Vertex_writer_position3x32_tex_coord2x32_normal1010102.hpp"
#include "Model/Model.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Math/Normalized_vector.hpp"

bool Vertex_writer_position3x32_tex_coord2x32_normal1010102::write_vertices(std::ostream &stream, const Model& model) const
{
	size_t num_vertices = model.positions.size();

	rendering::Vertex_position3x32_tex_coord2x32_normal1010102* vertices = new rendering::Vertex_position3x32_tex_coord2x32_normal1010102[num_vertices];

	for (size_t i = 0; i < num_vertices; ++i)
	{
		vertices[i].position  = model.positions[i];
		vertices[i].tex_coord = model.texture_coordinates[i];
		vertices[i].normal    = Vector4_UNorm1010102(model.normals[i]);
	}

	stream.write((char*)vertices, sizeof(rendering::Vertex_position3x32_tex_coord2x32_normal1010102) * num_vertices);

	delete [] vertices;

	return true;
}

const rendering::Vertex_layout_description* Vertex_writer_position3x32_tex_coord2x32_normal1010102::get_vertex_layout_description() const
{
	return rendering::Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description();
}
