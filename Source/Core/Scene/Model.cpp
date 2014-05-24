#include "Model.hpp"

namespace scene
{

Model::Model(const std::string& name) : Manageable(name), num_groups_(0), groups_(nullptr), m_vd(nullptr), vertex_buffers_(nullptr)
{}

Model::~Model()
{
	delete [] vertex_buffers_;

	delete [] groups_;
}

uint32_t Model::get_num_groups() const
{
	return num_groups_;
}

const Model::Group* Model::get_groups() const
{
	return groups_;
}

const rendering::Vertex_layout_description* Model::vertex_layout_description() const
{
	return m_vd;
}

const Handle<rendering::Vertex_buffer>* Model::vertex_buffers() const
{
	return vertex_buffers_;
}

const Handle<rendering::Index_buffer>& Model::index_buffer() const
{
	return index_buffer_;
}

uint32_t Model::num_indices() const
{
	return num_indices_;
}

size_t Model::num_bytes() const
{
	return 0;
}

}
