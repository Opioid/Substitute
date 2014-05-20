#include "Border_button.hpp"
#include "../Rendering/Printer.hpp"

namespace gui
{

Border_button::Border_button(Type type) : type_(type)
{
	size_ = float2(32.f, 32.f);
}

void Border_button::render(rendering::Printer& printer)
{
	printer.set_position(get_absolute_position());

	if (Type::Minimize == type_)
	{
		printer.set_texture_coordinates(float2(0.5f, 0.f), float2(1.f, 0.5f));
	}
	else if (Type::Close == type_)
	{
		printer.set_texture_coordinates(float2(0.f, 0.f), float2(0.5f, 0.5f));
	}

	if (m_enabled)
	{
		printer.set_color(m_hovered_over ? rendering::Color4(1.f, 0.f, 0.f) : rendering::Color4(0.f, 0.3f, 0.6f));
	}
	else
	{
		printer.set_color(rendering::Color4(0.01f, 0.01f, 0.015f));
	}
		
	printer.draw_quad(size_);
}

}
