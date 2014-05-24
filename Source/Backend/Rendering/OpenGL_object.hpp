#pragma once

#include "Resources/Reference_counter.hpp"
#include <cstdint>

namespace rendering
{

class OpenGL_object : public Reference_counter
{

public:

	OpenGL_object(uint32_t id);
//	virtual ~OpenGL_object();

	uint32_t id() const;

protected:

	const uint32_t id_;
};

}


