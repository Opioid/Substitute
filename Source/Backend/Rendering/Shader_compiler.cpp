#include "Shader_compiler.hpp"
#include "Rendering_device.hpp"
#include <GL/glew.h>

namespace rendering
{

Shader_compiler::Shader_compiler(const Rendering_device& device) : device_(device)
{}

Handle<Shader> Shader_compiler::compile(Shader::Type type, const std::string& source, std::string& error_message) const
{
	Handle<Shader> shader = device_.create_shader(type);

	uint32_t id = shader->id();

	const char* buffer = source.c_str();
	glShaderSource(id, 1, (const GLchar**)&buffer, nullptr);

	glCompileShader(id);

	int is_compiled;
	glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);

	if (!is_compiled)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* buffer = new char[length];
		glGetShaderInfoLog(id, length, nullptr, buffer);

		error_message = buffer;

		delete [] buffer;

		return Handle<Shader>(nullptr);
	}

	return shader;
}

}
