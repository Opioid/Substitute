#include "Surrounding.hpp"

namespace scene
{

Surrounding::Surrounding() : color_(0.1f, 0.1f, 0.15f)
{}

void Surrounding::clear()
{
    texture_ = nullptr;
}

const rendering::Color3& Surrounding::color() const
{
	return color_;
}

void Surrounding::set_color(const rendering::Color3& color)
{
	color_ = color;
}

const Handle<rendering::Shader_resource_view> Surrounding::texture() const
{
	return texture_;
}

void Surrounding::set_texture(const Handle<rendering::Shader_resource_view>& texture)
{
	texture_ = texture;
}

}
