#include "Import/Importer.hpp"
#include "Export/Exporter_json.hpp"
#include "Export/Exporter_bin.hpp"
#include "Model/Model.hpp"
#include "Math/Math.hpp"
#include "String/String.hpp"
#include <assimp/version.h>
#include <iostream>

void print_assimp_version();

int main()
{
	print_assimp_version();

	Importer importer;
//	Model* model = importer.read("sphere.dae");
	Model* model = importer.read("Rock.obj");
//	Model* model = importer.read("Sponza.obj");

	if (!model)
	{
		std::cout << "Model could not be imported." << std::endl;
		return 0;
	}

	model->scale(0.0139f);

	float3x3 rotation;
	set_rotation_x(rotation, math::to_radians(180.f));
	model->rotate(rotation);

//	model->set_origin_center_bottom();
	model->set_origin_center();

	AABB aabb = model->calculate_aabb();

	std::cout << aabb.position << std::endl;
	std::cout << aabb.halfsize << std::endl;

	Exporter_json exporter;

	exporter.write("Rock", *model);
//	exporter.write("Imrod", *model);

	delete model;

	std::cout << "Done." << std::endl;

	return 0;
}

void print_assimp_version()
{
	std::cout << "Using Assimp "  << aiGetVersionMajor() << "." << aiGetVersionMinor() << "." << aiGetVersionRevision();

	const unsigned int flags = aiGetCompileFlags();

	std::cout << (flags & ASSIMP_CFLAGS_DEBUG          ? " -debug"   : "");
	std::cout << (flags & ASSIMP_CFLAGS_NOBOOST        ? " -noboost" : "");
	std::cout << (flags & ASSIMP_CFLAGS_SHARED         ? " -shared"  : "");
	std::cout << (flags & ASSIMP_CFLAGS_SINGLETHREADED ? " -st"      : "");
	std::cout << (flags & ASSIMP_CFLAGS_STLPORT        ? " -stlport" : "");

	std::cout << std::endl;
}
