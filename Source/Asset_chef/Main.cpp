#include "Import/Importer.hpp"
#include "Export/Exporter.hpp"
#include "Export/Legacy_converter.hpp"
#include "Export/Vertex_writer_position3x32_tex_coord2x32_normal1010102.hpp"
#include "Model/Model.hpp"
#include "AABB_tree/AABB_tree.hpp"
#include "AABB_tree/AABB_tree_builder.hpp"
#include "AABB_tree/AABB_tree_writer.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>

bool export_model(const std::string& name, const Model& model);
bool export_AABB_tree(const std::string& name, const Model& model);

int main()
{
    std::cout << "Asset chef" << std::endl;

//    Legacy_converter converter;

//	if (!converter.convert("BananaTree.sum", "Glob/Data/Models/Banana_tree.sum"))
//    {
//		std::cout << "It didn't work." << std::endl;

//		return 0;
//    }


    Importer importer;

	Model* model = importer.read("Mitsuba.json");

    if (!model)
    {
        std::cout << "Model could not be imported." << std::endl;

        return 0;
    }

	export_model("Mitsuba", *model);
//	export_AABB_tree("Sponza", *model);

    delete model;

	std::cout << "Done." << std::endl;

    return 0;
}

bool export_model(const std::string& name, const Model& model)
{
    Exporter exporter;

    Vertex_writer_position3x32_tex_coord2x32_normal1010102 vertex_writer_normal;

    exporter.set_vertex_writer(&vertex_writer_normal);

    return exporter.write(name + ".sum", model);
}

bool export_AABB_tree(const std::string& name, const Model& model)
{
//	threading::Worker_pool worker_pool(6);

//	timing::Stop_watch watch;
//	watch.start();

	AABB_tree_builder builder(8 * 1024/*, worker_pool*/);

	AABB_tree* tree = builder.build(model);

	std::cout << "Built tree in " << /*watch.stop() <<*/ "x s" << std::endl;
	std::cout << "Vertices: " << builder.get_num_vertices() << std::endl;

	size_t triangle_overhead = builder.get_num_triangles() - builder.get_num_original_triangles();
	float triangle_inflation = math::round((float(triangle_overhead) / float(builder.get_num_original_triangles())) * 1000.f) / 10.f;

	std::cout << "Triangles: " << builder.get_num_triangles() << " (inflation: " << triangle_inflation << " %)" << std::endl;
	std::cout << "Clusters: " << builder.get_num_geometry_clusters() << std::endl;

	std::cout << std::endl << "Saving..." << std::endl;

	AABB_tree_writer writer;

	writer.write(name + ".abt", *tree);

	delete tree;

	return true;
}
