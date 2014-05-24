#include "Transformation_interpolator.hpp"
#include "Entity.hpp"
#include "Logging/Logging.hpp"
#include "String/String.hpp"

namespace scene
{

Transformation_interpolator::Transformation_interpolator(Entity* entity) : entity_(entity)
{}

void Transformation_interpolator::copy_previous_state()
{
	transformation_ = entity_->local_transformation();
}

void Transformation_interpolator::interpolate(float delta)
{
	float3 ip = lerp(transformation_.position, entity_->local_position(), delta);
	float3 is = lerp(transformation_.scale, entity_->local_scale(), delta);
	Quaternion ir = slerp(transformation_.rotation, entity_->local_rotation(), delta);

	entity_->set_world_transformation(ip, is, float3x3(ir));
}

}
