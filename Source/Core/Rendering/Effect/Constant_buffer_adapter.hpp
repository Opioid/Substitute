#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Data_buffer.hpp"

namespace rendering
{

class Shader_program;

class Constant_buffer_adapter
{

public:

	Constant_buffer_adapter(uint32_t binding_point, uint32_t num_bytes/*, uint32_t circular_size*/);
	~Constant_buffer_adapter();

	bool has_constant_buffer() const;
	void set_constant_buffer(const Handle<Constant_buffer>& constant_buffer);

	uint32_t get_binding_point() const;

	uint32_t get_num_bytes() const;

//	bool is_circular() const;

	void set(int offset, size_t num_bytes, const void* value);

	void bind(const Rendering_device& device);
//	void restart_circle();

	void update(const Rendering_device& device);
	void update(uint32_t num_bytes, const void* data, const Rendering_device& device);

private:

	Handle<Constant_buffer> constant_buffer_;

	uint32_t binding_point_;

	uint32_t num_bytes_;

//	uint32_t circular_size_;
//	uint32_t current_circle_segment_;
//	uint32_t circle_segment_size_;

	unsigned char* system_memory_;

	bool dirty_;

public:

	static void set_buffer_alignment(uint32_t alignment);

//	static uint32_t calculate_circle_segment_size(uint32_t num_bytes);

	static uint32_t s_buffer_alignment;
};

}


