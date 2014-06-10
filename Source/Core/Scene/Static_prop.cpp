#include "Static_prop.hpp"
#include "Model.hpp"
#include "Math/Matrix.inl"

namespace scene
{

Static_prop::Static_prop() : world_transformation_(float4x4::identity)
{}

float3 Static_prop::world_position() const
{
	return world_transformation_.rows[3].xyz;
}

float3 Static_prop::world_direction() const
{
	return normalize(world_transformation_.rows[2].xyz); 
}

float3 Static_prop::world_up() const
{
	return normalize(world_transformation_.rows[1].xyz); 
}

float3 Static_prop::world_right() const
{
	return normalize(world_transformation_.rows[0].xyz); 
}

float3x3 Static_prop::world_rotation() const
{
	return extract_unscaled_basis(world_transformation_);
}

const float4x4& Static_prop::world_transformation() const
{
	return world_transformation_;
}

void Static_prop::set_world_transformation(const float3& position, const float3& scale, const float3x3& rotation)
{
	set_basis(world_transformation_, rotation);
	::scale(world_transformation_, scale);
	set_origin(world_transformation_, position);

	model_->aabb().get_transformed(transformed_aabb_, world_transformation());
}

bool Static_prop::create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[])
{
    return Prop::create_surfaces(model, num_materials, materials, &world_transformation());
}

}
