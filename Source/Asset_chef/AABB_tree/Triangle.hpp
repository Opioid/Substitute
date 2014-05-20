#pragma once

#include "Rendering/Vertex_format.hpp"

template<typename T>
struct tTriangle
{
	T v[3];

	uint32_t material_id;

	tTriangle() {}
	tTriangle(const T &v0, const T &v1, const T &v2)
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
	}
};

typedef tTriangle<rendering::Vertex_position3x32_tex_coord2x32_normal3x32> Triangle;
