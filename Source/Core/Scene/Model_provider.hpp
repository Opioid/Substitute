#pragma once

#include "Resources/Resource_provider.hpp"
#include "Rendering/Vertex_layout_description.hpp"
#include "Model.hpp"

namespace rendering { class Rendering_tool; }

namespace scene
{

class Model_provider : public Resource_provider<Model>
{

public:

	Model_provider(rendering::Rendering_tool& rendering_tool);

	Handle<Model> load(file::Input_stream& stream, Resource_manager& resource_manager, uint32_t flags = 0) const;

private:

	Model* load_SUM(file::Input_stream& stream) const;

	const rendering::Vertex_layout_description* read_vertex_layout_description(std::istream& stream) const;

	void read_vertex_layout_description_element(std::istream& stream, rendering::Vertex_layout_description::Element& element) const;

	rendering::Rendering_tool& rendering_tool_;
};

}
