#include "Vertex_layout_cache.hpp"
#include "Rendering/Rendering_device.hpp"
#include "Rendering/Input_layout.hpp"

namespace rendering
{

Vertex_layout_cache::Vertex_layout_cache(const Rendering_device& device) : Cache(device)
{}

void Vertex_layout_cache::release()
{
	for (auto vd : vertex_descriptions_)
	{
		delete vd;
	}

	vertex_descriptions_.clear();

	for (auto l : input_layouts_)
	{
		delete l;
	}

	input_layouts_.clear();
}

const Vertex_layout_description* Vertex_layout_cache::vertex_layout_description(uint32_t num_elements_, const Vertex_layout_description::Element elements[])
{
	for (auto vd : vertex_descriptions_)
	{
		if (vd->consists_of(num_elements_, elements))
		{
			return vd;
		}
	}

	Vertex_layout_description* desc = new Vertex_layout_description(num_elements_, elements);

	vertex_descriptions_.push_back(desc);

	return desc;
}

Handle<Input_layout> Vertex_layout_cache::input_layout(const Vertex_layout_description& vd, const Shader_program::Signature& sig)
{
	for (auto l : input_layouts_)
	{
		if (l->is_compatible(vd, sig))
		{
			return l->input_layout;
		}
	}

	Handle<Input_layout> input_layout = device_.create_input_layout(vd, sig);

	if (!input_layout)
	{
		return nullptr;
	}

	input_layouts_.push_back(new Input_layout_slot{ vd, sig, input_layout });

	return input_layout;
}

bool Vertex_layout_cache::Input_layout_slot::is_compatible(const Vertex_layout_description& vd, const Shader_program::Signature& sig) const
{
	return vertex_description == vd && signature == sig;
}

}
