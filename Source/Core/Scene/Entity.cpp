#include "Entity.hpp"

namespace scene
{

Entity::Entity()
{
	state_.set(State::Visible, true);
}

bool Entity::is_visible() const
{
	return state_.is_set(State::Visible);
}

void Entity::set_visible(bool visible)
{
	state_.set(State::Visible, visible);
}

bool Entity::is_interpolated() const
{
	return state_.is_set(State::Interpolated);
}

void Entity::set_interpolated(bool interpolated)
{
	state_.set(State::Interpolated, interpolated);
}

}
