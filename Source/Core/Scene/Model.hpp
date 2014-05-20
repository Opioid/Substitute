#pragma once

#include "Prop.hpp"
#include "Resources/Resource.hpp"
#include "Resources/Manageable.hpp"
#include "Resources/Reference_counter.hpp"
#include "Resources/Handle.hpp"
#include "Math/Intersection/AABB.hpp"

namespace scene
{

class Model : public Resource, public Manageable, public Reference_counter
{

public:

	struct Group
	{
		uint32_t material_index;

		uint32_t start_index;
		uint32_t num_indices;
	};

	Model(const std::string& name);
	~Model();

	uint32_t get_num_groups() const;

	const Group* get_groups() const;

	const rendering::Vertex_layout_description* get_vertex_layout_description() const;

	const Handle<rendering::Vertex_buffer>* get_vertex_buffers() const;

	const Handle<rendering::Index_buffer>& get_index_buffer() const;

	uint32_t get_num_indices() const;

	inline const AABB& get_aabb() const
	{
		return aabb_;
	}

	size_t get_num_bytes() const;

private:

	uint32_t num_groups_;
	Group*   groups_;

	const rendering::Vertex_layout_description* m_vd;

	Handle<rendering::Vertex_buffer>* vertex_buffers_;
	Handle<rendering::Index_buffer>   index_buffer_;

	uint32_t num_vertices_;
	uint32_t num_indices_;

	AABB aabb_;

	friend class Model_provider;
};

}


