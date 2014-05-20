#include "Transformation_interpolator.hpp"
#include "Entity.hpp"
#include "../Logging/Logging.hpp"
#include "String/String.hpp"

namespace scene
{

Transformation_interpolator::Transformation_interpolator(Entity* entity) : m_entity(entity)
{}

void Transformation_interpolator::copy_previous_state()
{
	m_transformation = m_entity->get_local_transformation();
}

void Transformation_interpolator::interpolate(float delta)
{
	float3 ip = lerp(m_transformation.position, m_entity->get_local_position(), delta);
	float3 is = lerp(m_transformation.scale, m_entity->get_local_scale(), delta);
	Quaternion ir = slerp(m_transformation.rotation, m_entity->get_local_rotation(), delta);

	m_entity->set_world_transformation(ip, is, float3x3(ir));
}

}
