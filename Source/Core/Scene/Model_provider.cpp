#include "Model_provider.hpp"
#include "../Rendering/Rendering_tool.hpp"
#include "Rendering/Data_format.hpp"
#include "Rendering/Primitive_topology.hpp"
#include "String/String.hpp"
#include "Math/Math.hpp"
#include "File/Stream.hpp"
#include "../Logging/Logging.hpp"
#include <cstring>

namespace scene
{

Model_provider::Model_provider(rendering::Rendering_tool& rendering_tool) : Resource_provider("Model"), rendering_tool_(rendering_tool)
{}

Handle<Model> Model_provider::load(file::Input_stream& stream, Resource_manager& /*resource_manager*/, const Flags /*flags*/) const
{
	if (!stream)
	{
		logging::error("Model_provider::load_SUM(): \"" + stream.name() + "\" could not be loaded. File not found.");
		return nullptr;
	}

	Model* model = load_SUM(stream);

	if (!model)
	{
		logging::error("Model_provider::load(): \"" + stream.name() + "\" could not be loaded.");
		return nullptr;
	}

	return Handle<Model>(model);
}

Model* Model_provider::load_SUM(file::Input_stream& stream) const
{
	char header[3];
	stream.read(header, sizeof(char) * 3);

	if (strncmp("SUM", header, 3))
	{
		return nullptr;
	}

	unsigned char version = 0;
	stream.read((char*)&version, sizeof(unsigned char));

	if (4 != version)
	{
		return nullptr;
	}

	Model *model = new Model(stream.name());

	// Groups
	stream.read((char*)&model->num_groups_, sizeof(uint32_t));

	model->groups_ = new Model::Group[model->num_groups_];
	stream.read((char*)model->groups_, sizeof(Model::Group) * model->num_groups_);

	// Primitve toplogy
	rendering::Primitive_topology pt;
	stream.read((char*)&pt, sizeof(uint32_t));

	// Vertex description
	model->m_vd = read_vertex_layout_description(stream);

	// Vertices
	stream.read((char*)&model->num_vertices_, sizeof(uint32_t));

	model->vertex_buffers_ = new Handle<rendering::Vertex_buffer>[model->m_vd->num_streams()];

	for (uint32_t i = 0; i < model->m_vd->num_streams(); ++i)
	{
		const uint32_t num_vertex_bytes = model->m_vd->strides()[i] * model->num_vertices_;
		char *vertices = new char[num_vertex_bytes];
		stream.read(vertices, num_vertex_bytes);

		model->vertex_buffers_[i] = rendering_tool_.device().create_vertex_buffer(num_vertex_bytes, vertices);

		delete [] vertices;

		if (!model->vertex_buffers_[i])
		{
			delete model;
			return nullptr;
		}
	}

	// Indices
	rendering::Data_format::Value index_type;
	stream.read((char*)&index_type, sizeof(uint32_t));

	stream.read((char*)&model->num_indices_, sizeof(uint32_t));

	const uint32_t num_index_bytes = model->num_indices_ * rendering::Data_format::num_bytes_per_block(index_type);
	char* indices = new char[num_index_bytes];
	stream.read(indices, num_index_bytes);

	model->index_buffer_ = rendering_tool_.device().create_index_buffer(num_index_bytes, indices, index_type);

	delete [] indices;

	if (!model->index_buffer_)
	{
		delete model;
		return nullptr;
	}

	//AABB
	stream.read((char*)&model->aabb_, sizeof(AABB));

	bool skinned;
	stream.read((char*)&skinned, sizeof(bool));
/*
	if (skinned)
	{
		model->m_bindSkeleton = new Model::BindSkeleton;

		Model::BindSkeleton *skeleton = model->m_bindSkeleton;

		skeleton->m_root = new Model::BindNode;
		fillBindNode(skeleton->m_root, stream);

		stream.read((char*)&skeleton->m_numBoneOffsets, sizeof(uint32_t));

		//Final transformations of the nodes
		skeleton->m_boneOffsets = new float4x4[skeleton->m_numBoneOffsets];
		stream.read((char*)skeleton->m_boneOffsets, sizeof(float4x4) * skeleton->m_numBoneOffsets);

		//AABBs for the nodes
		skeleton->m_aabbs = new AABB[skeleton->m_numBoneOffsets];
		stream.read((char*)skeleton->m_aabbs, sizeof(AABB) * skeleton->m_numBoneOffsets);

		//AABB
		stream.read((char*)&skeleton->aabb_, sizeof(AABB));
	}
*/
	return model;
}

const rendering::Vertex_layout_description* Model_provider::read_vertex_layout_description(std::istream& stream) const
{
	uint32_t num_elements_;
	stream.read((char*)&num_elements_, sizeof(uint32_t));

	rendering::Vertex_layout_description::Element* elements = new rendering::Vertex_layout_description::Element[num_elements_];

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		stream >> elements[i];
	}

	const rendering::Vertex_layout_description* description = rendering_tool_.vertex_layout_cache().vertex_layout_description(num_elements_, elements);

	delete [] elements;

	return description;
}

void Model_provider::read_vertex_layout_description_element(std::istream& stream, rendering::Vertex_layout_description::Element& element) const
{
	element.semantic_name = file::read_string(stream);

	stream.read((char*)&element.semantic_index, sizeof(uint32_t));
	stream.read((char*)&element.format, sizeof(uint32_t));
	stream.read((char*)&element.slot, sizeof(uint32_t));
	stream.read((char*)&element.byte_offset, sizeof(uint32_t));
}

}
