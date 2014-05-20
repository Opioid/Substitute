#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Shader_program.hpp"

namespace rendering
{

class Effect;
class Rendering_device;

class Effect_technique
{

public:

	Effect_technique(const Handle<Shader_program>& program, const Effect* effect);

	void use() const;

	const Shader_program* get_program() const;
	Shader_program* get_program();

private:

	Handle<Shader_program> m_program;

	const Effect* effect_;
};

}

