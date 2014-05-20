#include "OpenGL_object.hpp"

namespace rendering
{

OpenGL_object::OpenGL_object(uint32_t id) : id_(id)
{}

/*
OpenGL_object::~OpenGL_object()
{}
*/

uint32_t OpenGL_object::get_id() const
{
	return id_;
}

}
