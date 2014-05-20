#pragma once

#include "Math/Vector.hpp"
#include <assimp/Importer.hpp>
#include <string>

struct Model;

class Importer
{

public:

	Model* read(const std::string& name);

private:

	Assimp::Importer importer_;

	static float4 tangent_and_bitangent_sign(const float3& normal, const float3& tangent, const float3& bitangent);
};
