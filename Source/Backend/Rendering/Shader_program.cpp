#include "Shader_program.hpp"
#include "Shader.hpp"
#include <GL/glew.h>

namespace rendering
{

Shader_program::Shader_program(uint32_t id) : OpenGL_object(id)
{}

Shader_program::~Shader_program()
{
	glDeleteProgram(id_);
}

bool Shader_program::link(std::vector<Handle<Shader>> shaders, std::string& error_message, std::map<std::string, std::string>* semantic_mapping)
{
	for (size_t i = 0; i < shaders.size(); ++i)
	{
		glAttachShader(id_, shaders[i]->id_);
	}

	glLinkProgram(id_);

	int is_linked;
	glGetProgramiv(id_, GL_LINK_STATUS, &is_linked);

	for (size_t i = 0; i < shaders.size(); ++i)
	{
		glDetachShader(id_, shaders[i]->id_);
	}

	if (!is_linked)
	{
		int length;
		glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &length);

		char* buffer = new char[length];
		glGetShaderInfoLog(id_, length, nullptr, buffer);

		error_message = buffer;

		delete [] buffer;

		return false;
	}

	int num_attributes;
	glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &num_attributes);

	signature_.resize(num_attributes);

	const uint32_t buffer_size = 64;
	char attribute_name[buffer_size];

	for (int i = 0; i < num_attributes; ++i)
	{
		int attribute_size;
		GLenum attribute_type;

		glGetActiveAttrib(id_, i, buffer_size, nullptr, &attribute_size, &attribute_type, attribute_name);

		int location = glGetAttribLocation(id_, attribute_name);

		signature_.elements_[i].location = location;

		if (semantic_mapping)
		{
			std::string an(attribute_name);

			auto semantic = semantic_mapping->find(an);

			if (semantic_mapping->end() != semantic)
			{
				signature_.elements_[i].semantic = semantic->second;
			}
		}
		else
		{
			signature_.elements_[i].semantic = std::string(attribute_name);
		}
	}

	return true;
}

void Shader_program::use() const
{
	glUseProgram(id_);
}

const Shader_program::Signature& Shader_program::signature() const
{
	return signature_;
}

uint32_t Shader_program::query_constant_buffer_size(const std::string& name) const
{
	uint32_t index = glGetUniformBlockIndex(id_, name.c_str());

	if (GL_INVALID_INDEX == index)
	{
		return 0;
	}

	int block_size = 0;
	glGetActiveUniformBlockiv(id_, index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

	return static_cast<uint32_t>(block_size);
}

bool Shader_program::Signature::Element::operator!=(const Element& b) const
{
	return location != b.location || semantic != b.semantic;
}

Shader_program::Signature::Signature() : num_elements_(0), elements_(nullptr)
{}

Shader_program::Signature::Signature(const Signature& signature) : num_elements_(signature.num_elements_), elements_(new Element[signature.num_elements_])
{
	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		elements_[i] = signature.elements_[i];
	}
}

Shader_program::Signature::~Signature()
{
	delete [] elements_;
}

Shader_program::Signature& Shader_program::Signature::operator=(const Signature& signature)
{
	num_elements_ = signature.num_elements_;

	delete [] elements_;
	elements_ = new Element[num_elements_];

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		elements_[i] = signature.elements_[i];
	}

	return *this;
}

bool Shader_program::Signature::operator==(const Signature& b) const
{
	if (num_elements_ != b.num_elements_)
	{
		return false;
	}

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		if (elements_[i] != b.elements_[i])
		{
			return false;
		}
	}

	return true;
}

uint32_t Shader_program::Signature::num_elements() const
{
	return num_elements_;
}

const Shader_program::Signature::Element& Shader_program::Signature::operator[](uint32_t index) const
{
	return elements_[index];
}

void Shader_program::Signature::resize(uint32_t num_elements)
{
	if (num_elements == num_elements_)
	{
		return;
	}

	delete [] elements_;

	num_elements_ = num_elements;

	elements_ = new Element[num_elements_];
}

}
