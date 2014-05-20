#pragma once

#include "Math/Vector.hpp"
#include "Rendering/Color.hpp"

struct half4;

namespace rendering
{

namespace baking
{

struct Ambient_cube;

class Ambient_cube_integrator
{

public:

	Ambient_cube_integrator(const uint2& face_dimensions);
	~Ambient_cube_integrator();

	void set_pitch(uint32_t pitch);

	void integrate(Ambient_cube& ambient_cube, uint32_t batch_index, const unsigned char* data) const;

private:

	Color3 integrate(uint32_t hemisphere, uint32_t batch_index, const half4* data) const;

	void save(const uint2& dimensions, uint32_t offset, const half4* source_data) const;

	uint2 face_dimensions_;

	uint32_t pitch_;

	uint2 weights_dimensions_;

	uint2 weights_offsets_[6];

	float* weights_;

	float summed_weights_;
};

}

}
