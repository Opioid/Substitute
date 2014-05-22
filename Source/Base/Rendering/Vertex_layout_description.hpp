#pragma once

#include "Data_format.hpp"
#include <fstream>
#include <string>
#include <cstdint>

namespace rendering
{

class Vertex_layout_description
{

public:

	struct Element;

	Vertex_layout_description(uint32_t num_elements, const Element elements[]);
	Vertex_layout_description(const Vertex_layout_description& other);
	Vertex_layout_description(Vertex_layout_description&& other);

//	Vertex_layout_description(Vertex_layout_description const& a, Vertex_layout_description const& b);
	~Vertex_layout_description();

	uint32_t get_num_elements_() const;
	const Element& operator[](uint32_t index) const;
	const Element* get_elements() const;

	uint32_t get_vertex_size() const;

	uint32_t* get_strides() const;
	uint32_t  get_num_streams() const;

	bool consists_of(uint32_t num_elements, const Element[]) const;
	bool operator==(const Vertex_layout_description& description) const;
	bool operator!=(const Vertex_layout_description& description) const;

	Vertex_layout_description operator+(const Vertex_layout_description& description) const;

private:

	uint32_t num_elements_;
	Element* elements_;

	uint32_t vertex_size_;

	uint32_t  num_streams_;
	uint32_t* strides_;

public:

	struct Classification
	{
		enum Value
		{
			Per_vertex_data,
			Per_instance_data
		};
	};

	struct Element
	{
		Element();
		Element(const char* semantic_name, uint32_t semantic_index, Data_format::Value format, uint32_t slot = 0, uint32_t byte_offset = 0xffffffff);
		Element(const char* semantic_name, uint32_t semantic_index, Data_format::Value format, Classification::Value classification, uint32_t step_rate, uint32_t slot = 0, uint32_t byte_offset = 0xffffffff);

		bool operator==(const Element& element) const;
		bool operator!=(const Element& element) const;

		std::string           semantic_name;
		uint32_t              semantic_index;
		Data_format::Value    format;
		uint32_t              slot;
		uint32_t              byte_offset;
		Classification::Value slot_class;
		uint32_t              instance_step_rate;
	};
};

}

std::istream& operator>>(std::istream& stream, rendering::Vertex_layout_description::Element& element);
