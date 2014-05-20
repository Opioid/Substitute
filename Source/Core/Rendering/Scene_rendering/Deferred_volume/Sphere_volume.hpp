#pragma once

#include "Deferred_volume.hpp"

namespace rendering
{

class Sphere_volume : public Deferred_volume
{

public:

	bool init(Rendering_tool& rendering_tool);

	void render(Rendering_tool& rendering_tool) const;
};

}
