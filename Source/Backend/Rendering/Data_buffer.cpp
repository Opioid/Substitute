#include "Data_buffer.hpp"
#include <GL/glew.h>

namespace rendering
{

Data_buffer::Data_buffer(uint32_t id, Type type, Usage usage, uint32_t size) : OpenGL_object(id), type_(type), size_(size)
{
	switch (usage)
	{
	case Usage::Dynamic:
		usage_ = GL_DYNAMIC_DRAW;
		break;
	case Usage::Stream:
		usage_ = GL_STREAM_DRAW;
		break;
	case Usage::Static:
		usage_ = GL_STATIC_DRAW;
		break;
	}
}

Data_buffer::~Data_buffer()
{
	glDeleteBuffers(1, &id_);
}

uint32_t Data_buffer::get_gl_type() const
{
	return get_gl_type(type_);
}

uint32_t Data_buffer::get_gl_type(Type type)
{
	static uint32_t gl_types[] = { GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER, 0 };

	return gl_types[static_cast<uint32_t>(type)];
}

Vertex_buffer::Vertex_buffer(uint32_t id, Usage usage, uint32_t size) : Data_buffer(id, Type::Vertex, usage, size)
{}

Index_buffer::Index_buffer(uint32_t id, Usage usage, uint32_t size, Data_format::Value format) :
	Data_buffer(id, Type::Index, usage, size),
	format_(format == Data_format::R16_UInt ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
	index_size_(static_cast<uint32_t>(Data_format::size_of(format)))
{}

Constant_buffer::Constant_buffer(uint32_t id, Usage usage, uint32_t size) : Data_buffer(id, Type::Constant, usage, size)
{}

}
