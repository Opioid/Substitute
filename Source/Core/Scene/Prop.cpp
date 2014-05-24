#include "Prop.hpp"
#include "Model.hpp"
#include "Rendering/Data_buffer.hpp"

#include <iostream>

namespace scene
{

Prop::Prop()
{}

void Prop::clear()
{
    model_ = nullptr;

    Renderable::clear();
}

const Model* Prop::model() const
{
	return model_;
}

bool Prop::create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[], const float4x4* world)
{
    if (!model || !num_materials || !materials[0])
	{
		return true;
	}

	model_ = model;

	unified_surface_.vd             = model_->vertex_layout_description();
	unified_surface_.start_index    = 0;
	unified_surface_.num_indices    = model_->num_indices();
	unified_surface_.vertex_buffers = model_->vertex_buffers();
	unified_surface_.index_buffer   = model_->index_buffer();
	unified_surface_.world          = world;
	unified_surface_.material       = materials[0];

	if (num_surfaces_ != model_->get_num_groups())
	{
		delete [] surfaces_;

		num_surfaces_ = model_->get_num_groups();

		surfaces_ = new Surface[num_surfaces_];
	}

	bool alpha_transparency = materials[0]->has_alpha_transparency();
	bool two_sided = materials[0]->is_two_sided();

	for (uint32_t i = 0; i < num_surfaces_; ++i)
	{
		Surface& s = surfaces_[i];

		const Model::Group& g = model_->get_groups()[i];

		s.vd = model_->vertex_layout_description();

		s.start_index = g.start_index;
		s.num_indices = g.num_indices;

		s.vertex_buffers = model_->vertex_buffers();
		s.index_buffer   = model_->index_buffer();

		s.world = unified_surface_.world;

		size_t material_index = g.material_index < num_materials ? g.material_index : num_materials - 1;

		s.material = materials[material_index];

		if (s.material->has_alpha_transparency() != alpha_transparency || s.material->is_two_sided() != two_sided )
		{
			set_mixed_render_states(true);
		}
	}

	return true;
}

const AABB& Prop::aabb() const
{
	return transformed_aabb_;
}

}
