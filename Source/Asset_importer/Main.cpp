#include "Import/Importer.hpp"
#include "Export/Exporter_json.hpp"
#include "Export/Exporter_bin.hpp"
#include "Model/Model.hpp"
#include "Math/Math.hpp"
#include "Math/Matrix.inl"
#include "String/String.hpp"
#include <assimp/version.h>
#include <iostream>
#include <sstream>
#include <iomanip>

void print_assimp_version();

int main()
{
	print_assimp_version();

	float3x3 rotation_x;
	set_rotation_x(rotation_x, math::to_radians(-90.f));


	float3x3 rotation_y;
	set_rotation_y(rotation_y, math::to_radians(180.f));

	float3x3 rotation_z;
	set_rotation_z(rotation_z, math::to_radians(90.f));

	float3x3 rotation = rotation_x;// * rotation_y;

//	for (size_t i = 0; i < 241; ++i) {
//		std::ostringstream istream;
//		istream << std::setw(5) << std::setfill('0');
//		istream << i + 1 << ".obj";

		Importer importer;
	//	Model* model = importer.read(/*istream.str()*/ "Bistro_Research_Exterior.fbx");
	//	Model* model = importer.read(/*istream.str()*/ "bistro_reset.fbx");
		Model* model = importer.read("shroom_8.fbx");

		if (!model)
		{
			std::cout << "Model could not be imported." << std::endl;
			return 0;
		}

		model->scale(0.01f);
	//	model->rotate(rotation);

		model->set_origin_center_bottom();
	//	model->set_origin_center();

		model->fix_tangent_frame();

		AABB aabb = model->calculate_aabb();

		std::cout << aabb.position << std::endl;
		std::cout << aabb.halfsize << std::endl;

		Exporter_json exporter;

//		std::ostringstream ostream;
//		ostream << "wiggle_";
//		ostream << std::setw(3) << std::setfill('0');
//		ostream << i;

		exporter.write(/*ostream.str()*/"shroom_8", *model);

		delete model;
//	}

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
