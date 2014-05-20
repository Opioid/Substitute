#include "Texture.hpp"
#include <GL/glew.h>

namespace rendering
{

Texture::Texture(uint32_t id, const Texture_description& description) :
	OpenGL_object(id),
	description_(description),
	internal_type_(get_gl_type(description.type))
{}

Texture::~Texture()
{
	if (description_.shader_resource)
	{
		glDeleteTextures(1, &id_);
	}
	else
	{
		glDeleteRenderbuffers(1, &id_);
	}
}

const Texture_description& Texture::get_description() const
{
	return description_;
}

uint32_t Texture::get_gl_type(Texture_description::Type type)
{
	switch (type)
	{
	case Texture_description::Type::Texture_1D:
		return GL_TEXTURE_1D;
	case Texture_description::Type::Texture_2D:
		return GL_TEXTURE_2D;
	case Texture_description::Type::Texture_3D:
		return GL_TEXTURE_3D;
	case Texture_description::Type::Texture_cube:
		return GL_TEXTURE_CUBE_MAP;
	case Texture_description::Type::Texture_2D_multisample:
		return GL_TEXTURE_2D_MULTISAMPLE;
	default:
		return 0;
	}
}

Texture_2D::Texture_2D(uint32_t id, const Texture_description& description) : Texture(id, description)
{}

Texture_3D::Texture_3D(uint32_t id, const Texture_description& description) : Texture(id, description)
{}

Texture_cube::Texture_cube(uint32_t id, const Texture_description& description) : Texture(id, description)
{
	description_.num_layers = 6;
}

Texture_2D_multisample::Texture_2D_multisample(uint32_t id, const Texture_description& description) : Texture(id, description)
{}

}
