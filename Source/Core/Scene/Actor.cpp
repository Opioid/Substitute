#include "Actor.hpp"
#include "Model.hpp"

namespace scene
{

Actor::Actor()
{}

Actor::~Actor()
{
}

void Actor::on_finalized_world_transformation()
{
	model_->get_aabb().get_transformed(transformed_aabb_, get_world_transformation());
}

bool Actor::create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[])
{
    return Prop::create_surfaces(model, num_materials, materials, &get_world_transformation());
}

}
