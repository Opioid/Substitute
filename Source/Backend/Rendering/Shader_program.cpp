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

	m_signature.resize(num_attributes);

	const uint32_t buffer_size = 64;
	char attribute_name[buffer_size];

	for (int i = 0; i < num_attributes; ++i)
	{
		int attribute_size;
		GLenum attribute_type;

		glGetActiveAttrib(id_, i, buffer_size, nullptr, &attribute_size, &attribute_type, attribute_name);

		int location = glGetAttribLocation(id_, attribute_name);

		m_signature.m_elements[i].location = location;

		if (semantic_mapping)
		{
			std::string an(attribute_name);

			auto semantic = semantic_mapping->find(an);

			if (semantic_mapping->end() != semantic)
			{
				m_signature.m_elements[i].semantic = semantic->second;
			}
		}
		else
		{
			m_signature.m_elements[i].semantic = std::string(attribute_name);
		}
	}

	return true;
}

void Shader_program::use() const
{
	glUseProgram(id_);
}

const Shader_program::Signature& Shader_program::get_signature() const
{
	return m_signature;
}

bool Shader_program::Signature::Element::operator!=(const Element& b) const
{
	return location != b.location || semantic != b.semantic;
}

Shader_program::Signature::Signature() : m_num_elements(0), m_elements(nullptr)
{}

Shader_program::Signature::Signature(const Signature& signature) : m_num_elements(signature.m_num_elements), m_elements(new Element[signature.m_num_elements])
{
	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		m_elements[i] = signature.m_elements[i];
	}
}

Shader_program::Signature::~Signature()
{
	delete [] m_elements;
}

Shader_program::Signature& Shader_program::Signature::operator=(const Signature& signature)
{
	m_num_elements = signature.m_num_elements;

	delete [] m_elements;
	m_elements = new Element[m_num_elements];

	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		m_elements[i] = signature.m_elements[i];
	}

	return *this;
}

bool Shader_program::Signature::operator==(const Signature& b) const
{
	if (m_num_elements != b.m_num_elements)
	{
		return false;
	}

	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		if (m_elements[i] != b.m_elements[i])
		{
			return false;
		}
	}

	return true;
}

uint32_t Shader_program::Signature::get_num_elements() const
{
	return m_num_elements;
}

const Shader_program::Signature::Element& Shader_program::Signature::operator[](uint32_t index) const
{
	return m_elements[index];
}

void Shader_program::Signature::resize(uint32_t num_elements)
{
	if (num_elements == m_num_elements)
	{
		return;
	}

	delete [] m_elements;

	m_num_elements = num_elements;

	m_elements = new Element[m_num_elements];
}

}
