#include "Effect_technique.hpp"
#include "Constant_buffer_adapter.hpp"

namespace rendering
{

Effect_technique::Effect_technique(const Handle<Shader_program>& program) : program_(program)
{}

void Effect_technique::use() const
{
	program_->use();
}

const Shader_program* Effect_technique::get_program() const
{
	return program_;
}

Shader_program* Effect_technique::get_program()
{
	return program_;
}

}
