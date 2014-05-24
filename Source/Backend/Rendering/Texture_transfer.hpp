#pragma once

#include "OpenGL_object.hpp"
#include "Rendering/Texture_description.hpp"

namespace rendering
{

class Rendering_device;

class Texture_transfer : public OpenGL_object
{

public:

	Texture_transfer(uint32_t id, const Texture_description& description);
	~Texture_transfer();

	const Texture_description& description() const;

private:

	Texture_description description_;

	unsigned char* buffer_;

	friend Rendering_device;
};

}


