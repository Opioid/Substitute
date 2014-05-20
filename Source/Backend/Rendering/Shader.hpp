#pragma once

#include "OpenGL_object.hpp"
#include <string>

namespace rendering
{

class Shader_program;

class Shader : public OpenGL_object
{

public:

	enum class Type
	{
		Vertex,
		Geometry,
		Pixel,
		Unknown
	};

	Shader(uint32_t id);
	~Shader();

	friend Shader_program;
};

}



