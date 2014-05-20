#pragma once

#include "Math/Vector.hpp"
#include "Rendering/Color.hpp"
#include "Rendering/Resource_view.hpp"
#include "Resources/Handle.hpp"

namespace scene
{

class Surrounding
{

public:

	Surrounding();

    void clear();

	const rendering::Color3& get_color() const;
	void set_color(const rendering::Color3& color);

	const Handle<rendering::Shader_resource_view> get_texture() const;
	void set_texture(const Handle<rendering::Shader_resource_view>& texture);

private:

	rendering::Color3 color_;

	Handle<rendering::Shader_resource_view> texture_;
};

}



