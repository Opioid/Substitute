#include "Importer.hpp"
#include "Model/Model.hpp"
#include "Math/Vector.inl"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <algorithm>

float3 float3_from_aiVector3(const aiVector3D& v);

Model* Importer::read(const std::string& name)
{
	importer_.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS);

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

	std::sort(meshes.begin(), meshes.end(), [] (aiMesh* a, aiMesh* b) -> bool
											{ 
												return a->mMaterialIndex < b->mMaterialIndex;
											} );

	Model* model = new Model;

	std::vector<uint32_t> group_vertex_offset;

	uint32_t num_vertices = 0;
	uint32_t num_indices = 0;

	for (size_t m = 0; m < meshes.size(); ++m)
	{
//		if (m >= 7)
//		{
//			break;
//		}

		const aiMesh& mesh = *meshes[m];

		Model::Group group;
		group.material_index = /*m;//*/mesh.mMaterialIndex;
		group.start_index = num_indices;
		group.num_indices = mesh.mNumFaces * 3;
		model->groups.push_back(group);
		group_vertex_offset.push_back(num_vertices);


		num_vertices += mesh.mNumVertices;
		num_indices  += group.num_indices;
	}

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
//			if (v >= 3609455 && v <= 3609599) {
//				continue;
//			}


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
		for (size_t f = 0; f < mesh.mNumFaces/* - 139*/; ++f)
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
