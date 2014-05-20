#pragma once

#include "../Math/Normalized_vector.hpp"
#include "../Math/Matrix.hpp"
#include "Vertex_layout_description.hpp"
#include <cstdint>

namespace rendering
{

struct Vertex_position2x32
{
	static const Vertex_layout_description* vertex_layout_description();
};

struct Vertex_position3x32
{
	static const Vertex_layout_description* vertex_layout_description();
};

struct Vertex_position3x32_color1x32
{
	float3       position;
	unsigned int color;

	static const Vertex_layout_description* vertex_layout_description();
};

struct Vertex_position2x32_tex_coord2x32
{
	float2 position;
	float2 tex_coord;

	static const Vertex_layout_description* vertex_layout_description();
};

struct Vertex_position3x32_tex_coord2x32
{
	float3 position;
	float2 tex_coord;

	static const Vertex_layout_description* vertex_layout_description();
};

struct Vertex_position3x32_normal3x32
{
	float3 position;
	float3 normal;

	static const Vertex_layout_description* vertex_layout_description();
};

/*
struct DistributedVertex_Pos3x32_Tex2x32
{
	static const Vertex_layout_description* vertex_layout_description();
};


struct InstancedVertex_Rows4x3x32
{
	float4x4 m_matrix;

	static const Vertex_layout_description* vertex_layout_description();
};
*/

struct Vertex_position3x32_tex_coord2x32_normal3x32
{
	float3 position;
	float2 tex_coord;
	float3 normal;

	static const Vertex_layout_description* vertex_layout_description();
};

/*
struct Vertex_textured_ntb
{
	float3 position;
	float2 m_tex;
	float3 m_norm;
	float3 m_tan;
	float3 m_bin;
};


struct Vertex_pos3x32_tex2x32_norm3x32_tan3x32_bias1x8
{
	float3      position;
	float2      m_tex;
	float3      m_norm;
	float3      m_tan;
	signed char m_bias;
};
*/

struct Vertex_position3x32_tex_coord2x32_normal1010102
{
	float3               position;
	float2               tex_coord;
	Vector4_UNorm1010102 normal;

	static const Vertex_layout_description* vertex_layout_description();
};

/*
struct DistributedVertex_Pos3x32_Tex2x32_Norm1010102
{
	static const Vertex_layout_description* vertex_layout_description();
};


struct Vertex_Pos3x32_Norm1010102_Ao1x8
{
	float3              position;
	Vector4_UNorm1010102 m_norm;
	unsigned char        m_ao;

	static const Vertex_layout_description* vertex_layout_description();
};


struct Vertex_Pos3x32_Tex2x32_Norm1010102_Tan1010102
{
	float3              position;
	float2              m_tex;
	Vector4_UNorm1010102 m_norm;
	Vector4_UNorm1010102 m_tan;

	static const Vertex_layout_description* vertex_layout_description();
};


struct DistributedVertex_Pos3x32_Tex2x32_Norm1010102_Tan1010102
{
	static const Vertex_layout_description* vertex_layout_description();
};


struct Vertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102
{
	float3              position;
	Vector4_4xUNorm8     m_weights;
	unsigned char        m_bones[4];
	float2              m_tex;
	Vector4_UNorm1010102 m_norm;

	static const Vertex_layout_description* vertex_layout_description();
};


struct DistributedVertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102
{
	static const Vertex_layout_description* vertex_layout_description();
};


struct DistributedVertex_Pos3x32_Weights4x8_Bones4x8_Norm1010102
{
	static const Vertex_layout_description* vertex_layout_description();
};


struct Vertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102_Tan1010102
{
	float3              position;
	Vector4_4xUNorm8     m_weights;
	unsigned char        m_bones[4];
	float2              m_tex;
	Vector4_UNorm1010102 m_norm;
	Vector4_UNorm1010102 m_tan;

	static const Vertex_layout_description* vertex_layout_description();
};


struct DistributedVertex_Pos3x32_Weights4x8_Bones4x8_Tex2x32_Norm1010102_Tan1010102
{
	static const Vertex_layout_description* vertex_layout_description();
};
*/

}
