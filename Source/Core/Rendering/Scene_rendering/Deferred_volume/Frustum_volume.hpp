#pragma once

#include "Deferred_volume.hpp"
#include "Math/Vector.hpp"

class Frustum;

namespace rendering
{

class Frustum_volume : public Deferred_volume
{

public:

	bool init(Rendering_tool& rendering_tool);

	void setup(const float3& pos, const Frustum& frustum, Rendering_tool& rendering_tool);

	void render(Rendering_tool& rendering_tool);
};


}
