#include "Constant_buffer_adapter.hpp"
#include "Rendering/Shader_program.hpp"
#include "Rendering/Rendering_device.hpp"
#include <cstring>

namespace rendering
{

Constant_buffer_adapter::Constant_buffer_adapter(uint32_t binding_point, uint32_t num_bytes/*, uint32_t circular_size*/) :
	binding_point_(binding_point), num_bytes_(num_bytes),
//	circular_size_(circular_size), current_circle_segment_(0), circle_segment_size_(calculate_circle_segment_size(num_bytes)),
	system_memory_(new unsigned char[num_bytes]), dirty_(false)
{}

Constant_buffer_adapter::~Constant_buffer_adapter()
{
	delete [] system_memory_;
}

bool Constant_buffer_adapter::has_constant_buffer() const
{
	return constant_buffer_ != nullptr;
}

void Constant_buffer_adapter::set_constant_buffer(const Handle<Constant_buffer>& constant_buffer)
{
	constant_buffer_ = constant_buffer;
}

uint32_t Constant_buffer_adapter::get_binding_point() const
{
	return binding_point_;
}

uint32_t Constant_buffer_adapter::get_num_bytes() const
{
	return num_bytes_;
}

//bool Constant_buffer_adapter::is_circular() const
//{
//	return circular_size_ > 1;
//}

void Constant_buffer_adapter::set(int offset, size_t num_bytes, const void* value)
{
	memcpy(system_memory_ + offset, value, num_bytes);

	dirty_ = true;
}

void Constant_buffer_adapter::bind(const Rendering_device& device)
{
	device.set_constant_buffer(constant_buffer_, binding_point_);
}

//void Constant_buffer_adapter::restart_circle()
//{
//	current_circle_segment_ = 0;
//}

void Constant_buffer_adapter::update(const Rendering_device& device)
{
	if (!dirty_)
	{
		return;
	}

	device.update_buffer(*constant_buffer_, 0, num_bytes_, system_memory_);

	dirty_ = false;
}

void Constant_buffer_adapter::update(uint32_t num_bytes, const void* data, const Rendering_device& device)
{
//	if (is_circular())
//	{
//		uint32_t offset = current_circle_segment_ * circle_segment_size_;

//		glBindBufferRange(GL_UNIFORM_BUFFER, binding_point_, constant_buffer_->get_id(), offset, num_bytes);
/*
		++current_circle_segment_;

		bool discard = false;

		if (current_circle_segment_ >= circular_size_)
		{
			discard = true;
			current_circle_segment_ = 0;
		}*/
/*
	//	device.update_buffer(*constant_buffer_, offset, num_bytes, data, discard);

		void* buffer;

		if (discard)
		{
			buffer = glMapNamedBufferRangeEXT(constant_buffer_->get_id(), offset, num_bytes, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		}
		else
		{
			buffer = glMapNamedBufferRangeEXT(constant_buffer_->get_id(), offset, num_bytes, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		}

		memcpy(buffer, data, num_bytes);

		glUnmapNamedBufferEXT(constant_buffer_->get_id());*/
//	}
//	else
	{
		device.update_buffer(*constant_buffer_, 0, num_bytes, data);
	}

	dirty_ = false;
}

void Constant_buffer_adapter::set_buffer_alignment(uint32_t alignment)
{
	s_buffer_alignment = alignment;
}

//uint32_t Constant_buffer_adapter::calculate_circle_segment_size(uint32_t num_bytes)
//{
//	uint32_t mod = num_bytes % s_buffer_alignment;

//	return mod ? num_bytes + (s_buffer_alignment - mod) : num_bytes;
//}

uint32_t Constant_buffer_adapter::s_buffer_alignment;

}
