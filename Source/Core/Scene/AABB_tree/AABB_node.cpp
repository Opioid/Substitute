#include "AABB_node.hpp"
#include "Rendering/Vertex_layout_description.hpp"

namespace scene
{

AABB_node::Geometry::Geometry() : Renderable(), vertex_buffers_(nullptr), num_vertices_(0), num_indices_(0)
{}

AABB_node::Geometry::~Geometry()
{
	delete [] vertex_buffers_;
}

bool AABB_node::Geometry::create_surfaces(uint32_t num_groups, Group* groups, const rendering::Vertex_layout_description* vd, const std::vector<Handle<Material>>& materials)
{
	if (!groups  || !materials.size() || !materials[0])
	{
		return true;
	}

	unified_surface_.vd             = vd;
	unified_surface_.start_index    = 0;
	unified_surface_.num_indices    = num_indices_;
	unified_surface_.vertex_buffers = vertex_buffers_;
	unified_surface_.index_buffer   = index_buffer_;
	unified_surface_.world          = &float4x4::identity;
	unified_surface_.material       = materials[0];

	if (num_surfaces_ != num_groups)
	{
		delete [] surfaces_;

		num_surfaces_ = num_groups;

		surfaces_ = new Surface[num_surfaces_];
	}

	bool alpha_transparency = materials[0]->has_alpha_transparency();
	bool two_sided = materials[0]->is_two_sided();

	for (uint32_t i = 0; i < num_surfaces_; ++i)
	{
		Surface& s = surfaces_[i];

		const Group& g = groups[i];

		s.vd = vd;

		s.start_index = g.start_index;
		s.num_indices = g.num_indices;

		s.vertex_buffers = vertex_buffers_;
		s.index_buffer   = index_buffer_;

		s.world = unified_surface_.world;

		size_t material_index = g.material_index < materials.size() ? g.material_index : materials.size() - 1;

		s.material = materials[material_index];

		if (s.material->has_alpha_transparency() != alpha_transparency || s.material->is_two_sided() != two_sided )
		{
			set_mixed_render_states(true);
		}
	}

	return true;
}

void AABB_node::Geometry::clear()
{
	for (uint32_t i = 0; i < unified_surface_.vd->num_streams(); ++i)
	{
		vertex_buffers_[i] = nullptr;
	}

	index_buffer_ = nullptr;

	Renderable::clear();
}

AABB_node::AABB_node() : skip_offset_(0), geometry_(nullptr)
{}

AABB_node::~AABB_node()
{}

void AABB_node::clear()
{
    props_.clear();

	geometry_ = nullptr;

	skip_offset_ = 0;
}

bool AABB_node::has_children() const
{ 
	return (skip_offset_ & s_has_children) == s_has_children;
}

const AABB_node* AABB_node::get_skip_node() const
{
	const uint32_t o = skip_offset_ & ~s_has_children;
    return o ? this + o : nullptr;
}

AABB_node* AABB_node::get_skip_node()
{
	const uint32_t o = skip_offset_ & ~s_has_children;
	return o ? this + o : nullptr; 
}

const AABB& AABB_node::aabb() const
{
	return aabb_;
}

const AABB_node::Geometry* AABB_node::get_geometry() const
{
	return geometry_;
}

const std::vector<Static_prop*>& AABB_node::get_static_props() const
{
	return props_;
}

void AABB_node::set_has_children(bool children)
{
	if (children)
	{
		skip_offset_ |= s_has_children;
	}
	else
	{
		skip_offset_ &= ~s_has_children;
	}
}

void AABB_node::set_skip_node(AABB_node* node)
{ 
	skip_offset_ |= node ? uint32_t(node - this) : 0;
}

}
