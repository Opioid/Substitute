#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Shader_program.hpp"

namespace rendering
{

class Rendering_device;

class Effect_technique
{

public:

	Effect_technique(const Handle<Shader_program>& program);

	void use() const;

	const Shader_program* program() const;
	Shader_program* program();

private:

	Handle<Shader_program> program_;
};

}

