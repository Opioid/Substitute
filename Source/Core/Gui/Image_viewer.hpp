#pragma once


#include "Frame.hpp"
#include "gui_Image.hpp"


namespace gui
{
	class Image_viewer : public Frame
	{

	public:

		Image_viewer(const Handle<rendering::Shader_resource_view>& texture, const std::string& image_name);

	protected:

		virtual void on_set_size();

	private:

		void render_private(rendering::Printer& printer);

		Image m_image;

	public:

		static void set_transparency_checker(const Handle<rendering::Shader_resource_view>& transparency_checker);

	private:

		static Handle<rendering::Shader_resource_view> s_transparency_checker;
	};
}
