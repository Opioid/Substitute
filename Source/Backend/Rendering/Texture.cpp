#include "Texture.hpp"
#include <algorithm>
#include <GL/glew.h>

namespace rendering
{

Texture::Texture(uint32_t id, const Texture_description& description) :
	OpenGL_object(id),
	description_(description),
	internal_type_(gl_type(description))
{
	description_.dimensions.z = std::max(description.dimensions.z, uint32_t(1));
}

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

const Texture_description& Texture::description() const
{
	return description_;
}

uint32_t Texture::gl_type(const Texture_description& description)
{
	switch (description.type)
	{
	case Texture_description::Type::Texture_1D:
		return GL_TEXTURE_1D;
	case Texture_description::Type::Texture_2D:
		return description.num_samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	case Texture_description::Type::Texture_3D:
		return GL_TEXTURE_3D;
	case Texture_description::Type::Texture_cube:
		return GL_TEXTURE_CUBE_MAP;
	default:
		return 0;
	}
}

Texture_2D::Texture_2D(uint32_t id, const Texture_description& description) : Texture(id, description)
{}

Texture_3D::Texture_3D(uint32_t id, const Texture_description& description) : Texture(id, description)
{}

Texture_cube::Texture_cube(uint32_t id, const Texture_description& description) : Texture(id, description)
{}

}
