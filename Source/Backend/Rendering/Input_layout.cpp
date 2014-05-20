#include "Input_layout.hpp"
#include <GL/glew.h>

namespace rendering
{

Input_layout::Input_layout(uint32_t id) : OpenGL_object(id)
{}

Input_layout::~Input_layout()
{
	glDeleteVertexArrays(1, &id_);
}

}
