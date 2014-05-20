#pragma once

#include "Shader.hpp"
#include "Resources/Handle.hpp"
#include <string>

namespace rendering
{

class Rendering_device;

class Shader_compiler
{

public:

	Shader_compiler(const Rendering_device& device);

	Handle<Shader> compile(Shader::Type type, const std::string& source, std::string& error_message) const;

private:

	const Rendering_device& device_;
};

}
