#include "gui_Component.hpp"
#include "Rendering/Resource_view.hpp"

namespace gui
{

Component::Component() : parent_(nullptr), position(float2::identity), m_has_focus(false), m_enabled(true)
{}

Component::~Component()
{}

void Component::set_parent(Component const* parent)
{
	parent_ = parent;
}

void Component::update(const float /*speed*/)
{}

void Component::update(const Gui_input& /*input*/)
{}

void Component::render(rendering::Printer& /*printer*/)
{}

float2 Component::get_absolute_position() const
{
	if (parent_)
	{
		return parent_->get_absolute_position() + position;
	}
	else
	{
		return position;
	}
}

void Component::set_position(const float2& pos)
{
	position = pos;
}

const float2& Component::dimensions() const
{
	return size_;
}

void Component::set_size(const float2& size)
{
	size_ = size;

	on_set_size();
}

bool Component::has_focus() const
{
	if (parent_)
	{
		return parent_->has_focus() && m_has_focus;
	}
	else
	{
		return m_has_focus;
	}
}

void Component::set_focus(bool focus)
{
	m_has_focus = focus;
}

bool Component::is_enabled() const
{
	if (parent_)
	{
		return parent_->is_enabled() && m_enabled;
	}
	else
	{
		return m_enabled;
	}
}

void Component::set_enabled(bool enabled)
{
	m_enabled = enabled;
}

void Component::on_set_size()
{}

Handle<rendering::Shader_resource_view> Component::s_texture;

void Component::set_texture(const Handle<rendering::Shader_resource_view>& texture)
{
	s_texture = texture;
}

}
