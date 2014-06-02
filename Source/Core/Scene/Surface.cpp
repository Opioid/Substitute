#include "Surface.hpp"
#include "Material.hpp"
#include "Rendering/Vertex_layout_description.hpp"

namespace scene
{

void Surface::clear()
{
	vertex_buffers = nullptr;

    index_buffer = nullptr;

    material = nullptr;
}

}
