#pragma once


#include "Resources/Handle.hpp"
#include "Surface.hpp"
#include "Flags/Flags.hpp"

namespace scene
{

class Material;

class Renderable
{

public:

	Renderable();
	~Renderable();

    void clear();

	const Material* get_material(uint32_t id) const;

	const Surface& unified_surface() const;

	uint32_t       num_surfaces() const;
	const Surface* surfaces() const;

	bool has_mixed_render_states() const;

protected:

	void set_mixed_render_states(bool value);

	Surface unified_surface_;

	uint32_t num_surfaces_;
	Surface* surfaces_;

	enum class Property
	{
		Mixed_render_states =  1 << 0
	};

	typedef Flags<Property, uint32_t> Renderable_properties;

	Renderable_properties properties_;
};

}

