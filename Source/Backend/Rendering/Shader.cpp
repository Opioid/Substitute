#include "Shader.hpp"
#include <GL/glew.h>

namespace rendering
{

Shader::Shader(uint32_t id) : OpenGL_object(id)
{}

Shader::~Shader()
{
	glDeleteShader(id_);
}

}
