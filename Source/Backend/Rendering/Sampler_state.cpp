#include "Sampler_state.hpp"
#include <GL/glew.h>

namespace rendering
{

Sampler_state::Sampler_state(uint32_t id) : OpenGL_object(id)
{}

Sampler_state::~Sampler_state()
{
	glDeleteSamplers(1, &id_);
}

Sampler_state::Description::Description() : filtering(Filtering::Point), max_anisotropy(1), address(Address::Repeat)
{}

bool Sampler_state::Description::operator==(const Description& other) const
{
	return filtering      == other.filtering
		&& max_anisotropy == other.max_anisotropy
		&& address        == other.address;
}

void Sampler_state::Description::Filtering::map(uint32_t& minification, uint32_t& magnification, Value filtering)
{
	switch (filtering)
	{
	default:
	case Point:
		minification = GL_NEAREST_MIPMAP_NEAREST;
		magnification = GL_NEAREST;
		break;
	case Bilinear:
		minification = GL_LINEAR_MIPMAP_NEAREST;
		magnification = GL_LINEAR;
		break;
	case Trilinear:
	case Anisotropic:
		minification = GL_LINEAR_MIPMAP_LINEAR;
		magnification = GL_LINEAR;
		break;
	}
}

uint32_t Sampler_state::Description::Address::map(Value address)
{
	switch (address)
	{
	default:
	case Repeat:
		return GL_REPEAT;
	case Clamp:
		return GL_CLAMP_TO_EDGE;
	}
}

}
