#include "Image_viewer.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Color.hpp"
#include "Rendering/Resource_view.hpp"
#include "File/File.hpp"

namespace gui
{

Image_viewer::Image_viewer(const Handle<rendering::Shader_resource_view>& texture, const std::string& image_name) : Frame(false), m_image(texture)
{
	set_caption(file::name_from_path(image_name));

	m_container.add(&m_image);

	//	set_client_size(float2(texture->get_resource().as_texture_2D().dimensions()));
}

void Image_viewer::on_set_size()
{
	Frame::on_set_size();

	m_image.set_size(get_client_size());
}

void Image_viewer::render_private(rendering::Printer& printer)
{
	printer.set_texture(s_transparency_checker);
	printer.set_color(rendering::color4::white);
	printer.set_position(get_absolute_position() + get_client_offset());
	printer.set_texture_coordinates(float2(0.f, 0.f), float2(m_image.dimensions() / 16.f));
	printer.draw_quad(get_client_size());
	printer.flush(false);
}

void Image_viewer::set_transparency_checker(const Handle<rendering::Shader_resource_view>& transparency_checker)
{
	s_transparency_checker = transparency_checker;
}

Handle<rendering::Shader_resource_view> Image_viewer::s_transparency_checker;

}
