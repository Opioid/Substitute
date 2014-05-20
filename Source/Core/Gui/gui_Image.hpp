#pragma once


#include "gui_Component.hpp"


namespace gui
{
	class Image : public Component
	{

	public:

		Image();
		Image(const Handle<rendering::Shader_resource_view>& texture);

		virtual void render(rendering::Printer& printer);

	private:

		Handle<rendering::Shader_resource_view> texture_;
	};
}
