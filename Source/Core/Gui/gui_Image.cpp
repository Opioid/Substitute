#include "gui_Image.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Color.hpp"
#include "Rendering/Resource_view.hpp"


namespace gui
{

Image::Image()
{}

Image::Image(const Handle<rendering::Shader_resource_view>& texture) : texture_(texture)
{}

void Image::render(rendering::Printer& printer)
{
	printer.set_texture(texture_);
	printer.set_color(rendering::color4::white);
	printer.set_position(get_absolute_position());
	printer.set_texture_coordinates();
	printer.draw_quad(size_);
	printer.flush(false);
}

}
