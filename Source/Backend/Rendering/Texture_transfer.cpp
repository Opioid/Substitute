#include "Texture_transfer.hpp"
#include <GL/glew.h>

namespace rendering
{

Texture_transfer::Texture_transfer(uint32_t id, const Texture_description& description) : OpenGL_object(id), description_(description)
{
	buffer_ = new unsigned char[description.dimensions.x * description.dimensions.y * Data_format::size_of(description.format)];
}

Texture_transfer::~Texture_transfer()
{
//	glDeleteBuffers(1, &id_);

	delete [] buffer_;
}

const Texture_description& Texture_transfer::get_description() const
{
	return description_;
}

}
