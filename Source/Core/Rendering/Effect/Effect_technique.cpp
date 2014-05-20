#include "Effect_technique.hpp"
#include "Effect.hpp"
#include "Constant_buffer_adapter.hpp"

namespace rendering
{

Effect_technique::Effect_technique(const Handle<Shader_program>& program, const Effect* effect) : m_program(program), effect_(effect)
{}

void Effect_technique::use() const
{
	m_program->use();
}

const Shader_program* Effect_technique::get_program() const
{
	return m_program;
}

Shader_program* Effect_technique::get_program()
{
	return m_program;
}

}
