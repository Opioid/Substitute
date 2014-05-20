#pragma once

#include "OpenGL_object.hpp"

namespace rendering
{

class Rendering_device;

class Sampler_state : public OpenGL_object
{

public:

	Sampler_state(uint32_t id);
	~Sampler_state();

	struct Description
	{
		Description();

		struct Filtering
		{
			enum Value
			{
				Point,
				Bilinear,
				Trilinear,
				Anisotropic
			};

			static void map(uint32_t& minification, uint32_t& magnification, Value filtering);
		};

		struct Address
		{
			enum Value
			{
				Repeat,
				Clamp
			};

			static uint32_t map(Value address);
		};

		bool operator==(const Description& other) const;

		Filtering::Value filtering;
		uint32_t max_anisotropy = 0;

		Address::Value address;
	};

	friend Rendering_device;
};

}



