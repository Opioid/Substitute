#include "Gui.hpp"
#include "Image_viewer.hpp"
#include "../Application/Cursor.hpp"
#include "Rendering/Resource_view.hpp"
#include "../Resources/Resource_manager.hpp"
#include "../Rendering/Printer.hpp"


namespace gui
{

Gui::Gui(logging::Message_server& message_server, scripting::Script_tool& Script_tool) : m_console(message_server, Script_tool), m_is_active(false)
{
	m_console.set_position(float2(128.f, 16.f));
	m_console.set_size(float2(1024.f, 640.f));

	clear();
}
	
bool Gui::is_active() const
{
	return m_is_active;
}

bool Gui::toggle_active()
{
	return m_is_active = !m_is_active;
}

bool Gui::init(Resource_manager& resource_manager, rendering::Printer& printer)
{
	Component::set_texture(resource_manager.load<rendering::Shader_resource_view>("Textures/Gui/Gui_skin.dds"));
	Image_viewer::set_transparency_checker(resource_manager.load<rendering::Shader_resource_view>("Textures/Gui/Transparency_checker.dds"));

	printer.set_font("Consola", 18);

	m_console.message_viewer().set_line_height(printer.line_height());

	return true;
}

void Gui::clear()
{
	m_frame_manager.clear();
	m_frame_manager.add(&m_console);
}

void Gui::add(Frame* frame)
{
	m_frame_manager.add(frame);
}

void Gui::update(const float speed)
{
	m_console.update(speed);
}

void Gui::update(const Gui_input& input)
{
	m_frame_manager.update(input);
}

void Gui::render(rendering::Printer& printer)
{
	m_frame_manager.render(printer);
}


void Gui::create_image_viewer(const Handle<rendering::Shader_resource_view>& texture, const std::string& image_name)
{
	Image_viewer* viewer = new Image_viewer(texture, image_name);

	viewer->set_position(float2(64.f, 64.f));

	m_frame_manager.add(viewer);
}

}
