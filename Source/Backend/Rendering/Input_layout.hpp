#pragma once

#include "OpenGL_object.hpp"
#include <bitset>

namespace rendering
{

class Rendering_device;

class Input_layout : public OpenGL_object
{

public:

	Input_layout(uint32_t id);
	virtual ~Input_layout();

private:

	std::bitset<8> m_attribute_states;

	friend Rendering_device;
};

}

