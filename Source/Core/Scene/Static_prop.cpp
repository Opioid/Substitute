#include "Static_prop.hpp"
#include "Model.hpp"

namespace scene
{

Static_prop::Static_prop() : world_transformation_(float4x4::identity)
{}

float3 Static_prop::get_world_position() const
{
	return world_transformation_.rows[3].xyz;
}

float3 Static_prop::get_world_dir() const
{
	return normalize(world_transformation_.rows[2].xyz); 
}

float3 Static_prop::get_world_up() const
{
	return normalize(world_transformation_.rows[1].xyz); 
}

float3 Static_prop::get_world_right() const
{
	return normalize(world_transformation_.rows[0].xyz); 
}

float3x3 Static_prop::get_world_rotation() const
{
	return get_unscaled_basis(world_transformation_);
}

const float4x4& Static_prop::get_world_transformation() const
{
	return world_transformation_;
}

void Static_prop::set_world_transformation(const float3& position, const float3& scale, const float3x3& rotation)
{
	set_basis(world_transformation_, rotation);
	::scale(world_transformation_, scale);
	set_origin(world_transformation_, position);

	model_->get_aabb().get_transformed(transformed_aabb_, get_world_transformation());
}

bool Static_prop::create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[])
{
    return Prop::create_surfaces(model, num_materials, materials, &get_world_transformation());
}

}
