#pragma once

#include "Entity.hpp"
#include "Prop.hpp"

namespace scene
{


class Actor : public Entity, public Prop
{

public:

	Actor();
	~Actor();

	void clear();

	void on_finalized_world_transformation();

    bool create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[]);
};

}
