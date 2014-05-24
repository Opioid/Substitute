#pragma once

#include "OpenGL_object.hpp"
#include "Rendering/Data_format.hpp"

namespace rendering
{

class Rendering_device;

class Data_buffer : public OpenGL_object
{

public:

	enum class Type
	{
		Vertex,
		Index,
		Constant,
		Type_count
	};

	enum class Usage
	{
		Static,
		Stream,
		Dynamic
	};

	Data_buffer(uint32_t id, Type type, Usage usage, uint32_t size);
	virtual ~Data_buffer();

private:

	uint32_t gl_type() const;

	Type type_;

	uint32_t usage_;

	uint32_t size_;

	static uint32_t gl_type(Type type);

	friend Rendering_device;
};

class Vertex_buffer : public Data_buffer
{

public:

	Vertex_buffer(uint32_t id, Usage usage, uint32_t size);

	friend Rendering_device;
};

class Index_buffer : public Data_buffer
{

public:

	Index_buffer(uint32_t id, Usage usage, uint32_t size, Data_format::Value format);

private:

	uint32_t format_;
	uint32_t index_size_;

	friend Rendering_device;
};

class Constant_buffer : public Data_buffer
{

public:

	Constant_buffer(uint32_t id, Usage usage, uint32_t size);

	friend Rendering_device;
};

}

