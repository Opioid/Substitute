#include "Importer.hpp"
#include "Model/Model.hpp"
#include "Math/Vector.inl"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>
#include <algorithm>

float3 float3_from_aiVector3(const aiVector3D& v);

void export_materials(const std::map<uint32_t, Model::Material>& materials) {
	std::ofstream stream("materials.txt");

	if (!stream) {
		return;
	}

	/*
	for (size_t i = 0, len = materials.size(); i < len; ++i) {
		stream << "\"" << materials[i].diffuse_texture << "\"";

		if (i < len - 1) {
			stream << ", ";
		}

		if (0 == (i + 1) % 8) {
			stream << std::endl;
		}
	}*/


	stream << "\"materials\": [" << std::endl;

	for (auto mp = materials.begin(); mp != materials.end();) {
		const auto& m = mp->second;

		if (materials.begin() == mp) {
			stream << "\t{" << std::endl;
		} else {
			stream << "{" << std::endl;
		}

//		stream << "\"" << materials[i].diffuse_texture << "\"";


		stream << "\t\t\"name\": \"" << m.name << "\",\n";

		stream << "\t\t\"rendering\": {\n";

		stream << "\t\t\t\"Substitute\": {\n";

		if (!m.diffuse_texture.empty()) {
			stream << "\t\t\t\t\"textures\": [\n";

			stream << "\t\t\t\t\t{\n";
			stream << "\t\t\t\t\t\t\"usage\": \"Color\"," << std::endl;
			stream << "\t\t\t\t\t\t\"file\": \"" << m.diffuse_texture.substr(9) << "\"\n";
			stream << "\t\t\t\t\t}";

			if (!m.mask_texture.empty()) {
				stream << ", {\n";
				stream << "\t\t\t\t\t\t\"usage\": \"Mask\"," << std::endl;
				stream << "\t\t\t\t\t\t\"file\": \"" << m.mask_texture.substr(9) << "\"\n";
				stream << "\t\t\t\t\t}";
			}

			if (!m.normal_texture.empty()) {
				stream << ", {\n";
				stream << "\t\t\t\t\t\t\"usage\": \"Normal\"," << std::endl;
				stream << "\t\t\t\t\t\t\"file\": \"" << m.normal_texture.substr(9) << "\"\n";
				stream << "\t\t\t\t\t}";
			}

			if (!m.spec_texture.empty()) {
				stream << ", {\n";
				stream << "\t\t\t\t\t\t\"usage\": \"Specularity\"," << std::endl;
				stream << "\t\t\t\t\t\t\"file\": \"" << m.spec_texture.substr(9) << "\"\n";
				stream << "\t\t\t\t\t}";
			}

			stream << std::endl << "\t\t\t\t],\n";
		} else {
			const auto& c = m.diffuse_color;
			stream << "\t\t\t\t\"color\": [" << c.x << ", " << c.y << ", " << c.z << "],\n";
		}

		stream << "\t\t\t\t\"roughness\": " << m.roughness;

		if (!m.mask_texture.empty()) {
			stream << ",\n\t\t\t\t\"two_sided\": true";
		}

		stream << "\n\t\t\t}\n";

		stream << "\t\t}\n";

		stream << "\t}";

		++mp;
		if (materials.end() != mp) {
			stream << ", ";
		} else {
			stream << std::endl;
		}
	}

	stream << "], \n";


	stream << "\"materials\": [" << std::endl;
/*
	for (const auto& g : model.groups) {
		const auto& m = model.materials.at(g.material_index);

		stream << "\"" << m.name << "\"";

	//	if (model.groups.back() != g) {
			stream << ",";
	//	}

		stream << std::endl;
	}
*/
	for (auto mp = materials.begin(); mp != materials.end();) {
		const auto& m = mp->second;

		stream << "\"" << m.name << "\"";

		++mp;
		if (materials.end() != mp) {
			stream << ",";
		}

		stream << std::endl;
	}

	stream << "]\n";


	std::cout << "Exported materials" << std::endl;
}


float shininess_to_roughness(float shininess) {
	return std::pow(2.f / (shininess + 2.f), 0.25f);
}

Model* Importer::read(const std::string& name)
{
	importer_.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS /*| aiComponent_NORMALS*/);

	aiScene const* scene = importer_.ReadFile(name,
		  aiProcess_ConvertToLeftHanded
		| aiProcess_RemoveComponent
		| aiProcess_Triangulate
		| aiProcess_RemoveRedundantMaterials
		| aiProcess_PreTransformVertices
		| aiProcess_JoinIdenticalVertices
		| aiProcess_FixInfacingNormals
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
	//	| aiProcess_ImproveCacheLocality
		| aiProcess_OptimizeMeshes
		| aiProcess_OptimizeGraph
	);

	if (!scene)
	{
		std::cout << "Could not read \"" << name << "\". " << importer_.GetErrorString() << std::endl;
		return nullptr;
	}

	std::vector<aiMesh*> meshes;

	for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
//		if (i > 0) {
//			continue;
//		}

		meshes.push_back(scene->mMeshes[i]);
	}

//	std::sort(meshes.begin(), meshes.end(), [scene] (aiMesh* a, aiMesh* b) -> bool
//											{
//											//	return a->mMaterialIndex < b->mMaterialIndex;


//												aiString a_path;
//												scene->mMaterials[a->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &a_path);

//												aiString b_path;
//												scene->mMaterials[b->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &b_path);

//												std::string a_string = a_path.data;
//												std::string b_string = b_path.data;

//												return a_string < b_string;
//											} );

	Model* model = new Model;

	std::vector<uint32_t> group_vertex_offset;

	uint32_t num_vertices = 0;
	uint32_t num_indices = 0;

	for (size_t m = 0; m < meshes.size(); ++m)
	{
		const aiMesh& mesh = *meshes[m];

		Model::Group group;
		group.material_index = /*m;//*/mesh.mMaterialIndex;
	//	std::cout << group.material_index << std::endl;
		group.start_index = num_indices;
		group.num_indices = mesh.mNumFaces * 3;
		model->groups.push_back(group);
		group_vertex_offset.push_back(num_vertices);


		num_vertices += mesh.mNumVertices;
		num_indices  += group.num_indices;



		const auto& mi = model->materials.find(group.material_index);
		if (model->materials.end() == mi) {
			const auto& material = *scene->mMaterials[group.material_index];

			std::string material_name;
			aiString text;
			if (aiReturn_SUCCESS == material.Get(AI_MATKEY_NAME, text)) {
				material_name = text.data;
			}

			std::string diffuse_texture_name;
			aiString path;
			if (aiReturn_SUCCESS == material.GetTexture(aiTextureType_DIFFUSE, 0, &path)) {
				diffuse_texture_name = path.data;
				std::replace(diffuse_texture_name.begin(), diffuse_texture_name.end(), '\\', '/');
			}

			std::string mask_texture_name;
			if (aiReturn_SUCCESS == material.GetTexture(aiTextureType_OPACITY, 0, &path)) {
				mask_texture_name = path.data;
				std::replace(mask_texture_name.begin(), mask_texture_name.end(), '\\', '/');
			}

			std::string normal_texture_name;
			if (aiReturn_SUCCESS == material.GetTexture(aiTextureType_NORMALS, 0, &path)) {
				normal_texture_name = path.data;
				std::replace(normal_texture_name.begin(), normal_texture_name.end(), '\\', '/');
			}

			std::string spec_texture_name;
			if (aiReturn_SUCCESS == material.GetTexture(aiTextureType_SPECULAR, 0, &path)) {
				spec_texture_name = path.data;
				std::replace(spec_texture_name.begin(), spec_texture_name.end(), '\\', '/');
			}

			/*)
			aiColor3D transparent_color;
			if (aiReturn_SUCCESS == scene->mMaterials[group.material_index]->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent_color)) {
			//	material_name = text.data;

				std::cout << "transparent color" << std::endl;
			}*/

			aiColor3D diffuse_color;
			if (aiReturn_SUCCESS == scene->mMaterials[group.material_index]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color)) {
			//	material_name = text.data;

				std::cout << "diffuse color" << std::endl;


			}

			float shininess = -1.f;
			material.Get(AI_MATKEY_SHININESS, shininess);

			float roughness = 0.8f;
			if (shininess > 0.f) {
				roughness = shininess_to_roughness(shininess);
			}

			float opacity = 1.f;
			material.Get(AI_MATKEY_OPACITY, opacity);

			int two_sided = 0;
			material.Get(AI_MATKEY_TWOSIDED, two_sided);

			model->materials.emplace(group.material_index,
									 Model::Material{
										material_name,
										diffuse_texture_name, mask_texture_name,
										normal_texture_name, spec_texture_name,
										float3(diffuse_color.r, diffuse_color.g, diffuse_color.b),
										roughness, opacity, 0 != two_sided});
		}
	}

	export_materials(model->materials);

	const bool has_positions = scene->mMeshes[0]->HasPositions();
	const bool has_texture_coordinates = scene->mMeshes[0]->HasTextureCoords(0);
	const bool has_normals = scene->mMeshes[0]->HasNormals();
	const bool has_tangent_space = has_normals & scene->mMeshes[0]->HasTangentsAndBitangents();

	if (has_positions)
	{
		model->positions.resize(num_vertices);
	}

	if (has_texture_coordinates)
	{
		model->texture_coordinates.resize(num_vertices);
	}

	if (has_normals)
	{
		model->normals.resize(num_vertices);
	}

	if (has_tangent_space)
	{
		model->tangents_and_bitangent_signs.resize(num_vertices);
	}

	model->indices.resize(num_indices);

	uint32_t current_vertex = 0;
	uint32_t current_index = 0;

	for (size_t m = 0; m < model->groups.size(); ++m)
	{
		const aiMesh& mesh = *meshes[m];

		//copy per vertex data
		for (uint32_t v = 0; v < mesh.mNumVertices; ++v, ++current_vertex)
		{
			if (has_positions && mesh.HasPositions())
			{
				model->positions[current_vertex] = float3_from_aiVector3(mesh.mVertices[v]);
			}

			if (has_texture_coordinates && mesh.HasTextureCoords(0))
			{
				model->texture_coordinates[current_vertex] = float2(mesh.mTextureCoords[0][v].x, mesh.mTextureCoords[0][v].y);
			}

			if (has_normals && mesh.HasNormals())
			{
				model->normals[current_vertex] = float3_from_aiVector3(mesh.mNormals[v]);
			}

			if (has_tangent_space && mesh.HasTangentsAndBitangents())
			{
				const float3 tangent   = float3_from_aiVector3(mesh.mTangents[v]);
				const float3 bitangent = float3_from_aiVector3(mesh.mBitangents[v]);
				model->tangents_and_bitangent_signs[current_vertex] = tangent_and_bitangent_sign(model->normals[current_vertex], tangent, -bitangent);
			}
		}

		// copy the indices
		for (size_t f = 0; f < mesh.mNumFaces; ++f)
		{
			// after triangulation this should always by 3
			for (size_t i = 0; i < mesh.mFaces[f].mNumIndices; ++i, ++current_index)
			{
				size_t index = group_vertex_offset[m] + mesh.mFaces[f].mIndices[i];

//				if (index > 3609599) {
//					index -= 144;
//				}

				model->indices[current_index] = index;
			}
		}
	}

	return model;
}

float4 Importer::tangent_and_bitangent_sign(const float3& normal, const float3& tangent, const float3& bitangent)
{
	const float3 temp = cross(normal, tangent);

	float d = dot(temp, bitangent);

	return float4(tangent, d > 0.f ? 1.f : -1.f);
}

float3 float3_from_aiVector3(const aiVector3D& v)
{
	float x = std::isnan(v.x) || std::isinf(v.x) ? 0.f : v.x;
	float y = std::isnan(v.y) || std::isinf(v.y) ? 0.f : v.y;
	float z = std::isnan(v.z) || std::isinf(v.z) ? 0.f : v.z;

	return float3(x, y, z);
}
