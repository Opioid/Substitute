#pragma once

#include "Button.hpp"

namespace gui
{

class Border_button : public Button
{

public:

	enum class Type
	{
		Minimize,
		Close
	};

	Border_button(Type type);

	void render(rendering::Printer& printer);

private:

	Type type_;

};
}
