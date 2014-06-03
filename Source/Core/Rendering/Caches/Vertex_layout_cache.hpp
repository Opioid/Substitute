#pragma once

#include "Cache.hpp"
#include "Rendering/Vertex_layout_description.hpp"
#include "Rendering/Shader_program.hpp"
#include "Resources/Handle.hpp"
#include <list>
#include <cstdint>

namespace rendering
{

class Input_layout;

class Vertex_layout_cache : public Cache
{

public:

	Vertex_layout_cache(const Rendering_device& device);

	virtual void release();

	const Vertex_layout_description* vertex_layout_description(uint32_t num_elements_, const Vertex_layout_description::Element elements[]);

	Handle<Input_layout> input_layout(const Vertex_layout_description& vertex_description, const Shader_program::Signature& signature);

private:

	struct Input_layout_slot
	{
		bool is_compatible(const Vertex_layout_description& vd, const Shader_program::Signature& signature) const;

		Vertex_layout_description vertex_description;

		Shader_program::Signature signature;

		Handle<Input_layout> input_layout;
	};

	std::list<const Vertex_layout_description*> vertex_descriptions_;
	std::list<const Input_layout_slot*> input_layouts_;
};

}
