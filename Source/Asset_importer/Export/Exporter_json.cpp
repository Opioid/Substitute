#include "Exporter_json.hpp"
#include "Model/Model.hpp"
#include <fstream>

#include <iostream>

bool Exporter_json::write(const std::string& name, const Model& model) const
{
	std::ofstream stream(name + ".json");

	if (!stream)
	{
		return false;
	}

	stream << "{" << std::endl;

	stream << "\t\"geometry\":" << std::endl;
	stream << "\t{" << std::endl;

	//Groups
	stream << "\t\t\"groups\":" << std::endl;
	stream << "\t\t[" << std::endl;

	for (size_t i = 0; i < model.groups.size(); ++i)
	{
		stream << "\t\t\t{" << std::endl;

		stream << "\t\t\t\t\"material_index\": ";
		stream << model.groups[i].material_index;
		stream << "," << std::endl;

		stream << "\t\t\t\t\"start_index\": ";
		stream << model.groups[i].start_index;
		stream << "," << std::endl;

		stream << "\t\t\t\t\"num_indices\": ";
		stream << model.groups[i].num_indices;
		stream << std::endl;

		stream << "\t\t\t}";

		if (i < model.groups.size() - 1)
		{
			stream << "," << std::endl;
		}
	}

	stream << std::endl;

	stream << "\t\t]," << std::endl << std::endl;

	//Primitive Topology
	stream << "\t\t\"primitive_topology\": \"triangle_list\"," << std::endl << std::endl;

	//Positions
	if (model.has_positions())
	{
		stream << "\t\t\"positions\": " << std::endl;
		stream << "\t\t[" << std::endl;

		stream << "\t\t\t";

		for (size_t i = 0; i < model.positions.size(); ++i)
		{
			stream << "[" << model.positions[i].x << "," << model.positions[i].y << "," << model.positions[i].z << "]";

			if (i < model.positions.size() - 1)
			{
				stream << ",";
			}

			if ((i + 1) % 8 == 0)
			{
				stream << std::endl << "\t\t\t";
			}
		}

		stream << std::endl;

		stream << "\t\t]," << std::endl << std::endl;
	}

	//Texture_2D Coordinates
	if (model.has_texture_coordinates())
	{
		stream << "\t\t\"texture_coordinates_0\": " << std::endl;
		stream << "\t\t[" << std::endl;

		stream << "\t\t\t";

		for (size_t i = 0; i < model.texture_coordinates.size(); ++i)
		{
			stream << "[" << model.texture_coordinates[i].x << "," << model.texture_coordinates[i].y << "]";

			if (i < model.texture_coordinates.size() - 1)
			{
				stream << ",";
			}

			if ((i + 1) % 8 == 0)
			{
				stream << std::endl << "\t\t\t";
			}
		}

		stream << std::endl;

		stream << "\t\t]," << std::endl << std::endl;
	}

	//Normals
	if (model.has_normals())
	{
		stream << "\t\t\"normals\": " << std::endl;
		stream << "\t\t[" << std::endl;

		stream << "\t\t\t";

		for (size_t i = 0; i < model.normals.size(); ++i)
		{
			stream << "[" << model.normals[i].x << "," << model.normals[i].y << "," << model.normals[i].z << "]";

			if (i < model.normals.size() - 1)
			{
				stream << ",";
			}

			if ((i + 1) % 8 == 0)
			{
				stream << std::endl << "\t\t\t";
			}
		}

		stream << std::endl;

		stream << "\t\t]," << std::endl << std::endl;
	}

	//Tangent Space
	if (model.has_tangent_space())
	{
		//Tangents
		stream << "\t\t\"tangents_and_bitangent_signs\": " << std::endl;
		stream << "\t\t[" << std::endl;

		stream << "\t\t\t";

		for (size_t i = 0; i < model.tangents_and_bitangent_signs.size(); ++i)
		{
			stream << "[" << model.tangents_and_bitangent_signs[i].x << ","
						  << model.tangents_and_bitangent_signs[i].y << ","
						  << model.tangents_and_bitangent_signs[i].z << ","
						  << model.tangents_and_bitangent_signs[i].w << "]";

			if (i < model.tangents_and_bitangent_signs.size() - 1)
			{
				stream << ",";
			}

			if ((i + 1) % 8 == 0)
			{
				stream << std::endl << "\t\t\t";
			}
		}

		stream << std::endl;

		stream << "\t\t]," << std::endl << std::endl;
	}

	//Indices
	stream << "\t\t\"indices\": " << std::endl;
	stream << "\t\t[" << std::endl;

	stream << "\t\t\t";

	for (size_t i = 0; i < model.indices.size(); ++i)
	{
		stream << model.indices[i];

		if (i < model.indices.size() - 1)
		{
			stream << ",";
		}

		if ((i + 1) % 8 == 0)
		{
			stream << std::endl << "\t\t\t";
		}
	}

	stream << std::endl;

	stream << "\t\t]" << std::endl;

	stream << "\t}" << std::endl;

	stream << "}";

	return true;
}
