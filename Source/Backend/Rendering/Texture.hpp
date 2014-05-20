#pragma once

#include "OpenGL_object.hpp"
#include "Rendering/Texture_description.hpp"

namespace rendering
{

class Rendering_device;

class Texture : public OpenGL_object
{

public:

	Texture(uint32_t id, const Texture_description& description);
	virtual ~Texture();

	const Texture_description& get_description() const;

	static uint32_t get_gl_type(Texture_description::Type type);

protected:

	Texture_description description_;

	uint32_t internal_type_;

	friend Rendering_device;
};

class Texture_2D : public Texture
{

public:

	Texture_2D(uint32_t id, const Texture_description& description);
};

class Texture_3D : public Texture
{

public:

	Texture_3D(uint32_t id, const Texture_description& description);
};

class Texture_cube : public Texture
{

public:

	Texture_cube(uint32_t id, const Texture_description& description);
};

class Texture_2D_multisample : public Texture
{

public:

	Texture_2D_multisample(uint32_t id, const Texture_description& description);
};

}
