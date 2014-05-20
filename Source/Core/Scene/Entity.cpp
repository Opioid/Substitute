#include "Entity.hpp"

namespace scene
{

Entity::Entity() : m_flags(Flags::Visible)
{}

bool Entity::is_visible() const
{
	return (m_flags & Flags::Visible) == Flags::Visible;
}

void Entity::set_visible(bool visible)
{
	visible ? m_flags |= Flags::Visible : m_flags &= ~Flags::Visible;
}

bool Entity::is_interpolated() const
{
	return (m_flags & Flags::Interpolated) == Flags::Interpolated;
}

void Entity::set_interpolated(bool interpolated)
{
	interpolated ? m_flags |= Flags::Interpolated : m_flags &= ~Flags::Interpolated;
}


}
