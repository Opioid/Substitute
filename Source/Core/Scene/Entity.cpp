#include "Entity.hpp"

namespace scene
{

Entity::Entity() : flags_(Flags::Visible)
{}

bool Entity::is_visible() const
{
	return (flags_ & Flags::Visible) == Flags::Visible;
}

void Entity::set_visible(bool visible)
{
	visible ? flags_ |= Flags::Visible : flags_ &= ~Flags::Visible;
}

bool Entity::is_interpolated() const
{
	return (flags_ & Flags::Interpolated) == Flags::Interpolated;
}

void Entity::set_interpolated(bool interpolated)
{
	interpolated ? flags_ |= Flags::Interpolated : flags_ &= ~Flags::Interpolated;
}


}
