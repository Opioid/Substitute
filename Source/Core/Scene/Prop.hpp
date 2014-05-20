#pragma once

#include "Renderable.hpp"
#include "Math/Intersection/AABB.hpp"
#include <vector>

namespace scene
{

class Model;

class Prop : public Renderable
{

public:

	Prop();

    void clear();

	const Model* get_model() const;

	const AABB& get_aabb() const;

protected:

    bool create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[], const float4x4* world);

	Handle<Model> model_;

	AABB transformed_aabb_;
};

}
