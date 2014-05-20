#include "Vertex_layout_description.hpp"
#include "File/Stream.hpp"
#include <algorithm>
#include <cstring>
#include <boost/algorithm/string.hpp>   

namespace rendering
{

Vertex_layout_description::Vertex_layout_description(uint32_t num_elements, const Element elements[]) : m_num_elements(num_elements), m_vertex_size(0) , m_num_streams(0)
{
	m_elements = new Element[m_num_elements];

	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		m_elements[i] = elements[i];
		m_num_streams = std::max(m_num_streams, m_elements[i].slot);
	}

	++m_num_streams;
	m_strides = new uint32_t[m_num_streams];

	std::fill_n(m_strides, m_num_streams, 0);

	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		if (0xffffffff == m_elements[i].byte_offset)
		{
			m_elements[i].byte_offset = m_strides[m_elements[i].slot];
		}
		else
		{
			m_strides[m_elements[i].slot] = m_elements[i].byte_offset;
		}

		m_strides[m_elements[i].slot] += uint32_t(Data_format::size_of(m_elements[i].format));
	}

	for (uint32_t i = 0; i < m_num_streams; ++i)
	{
		m_vertex_size += m_strides[i];
	}
}

Vertex_layout_description::Vertex_layout_description(const Vertex_layout_description& other)
	: m_num_elements(other.m_num_elements), m_elements(new Element[other.m_num_elements]), m_vertex_size(other.m_vertex_size),
	  m_num_streams(other.m_num_streams), m_strides(new uint32_t[other.m_num_streams])
{
	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		m_elements[i] = other.m_elements[i];
	}

	for (uint32_t i = 0; i < m_num_streams; ++i)
	{
		m_strides[i] = other.m_strides[i];
	}
}

Vertex_layout_description::Vertex_layout_description(Vertex_layout_description&& other)
	: m_num_elements(other.m_num_elements), m_elements(other.m_elements), m_vertex_size(other.m_vertex_size),
	  m_num_streams(other.m_num_streams), m_strides(other.m_strides)
{
	other.m_num_elements = 0;
	other.m_elements = nullptr;

	other.m_num_streams = 0;
	other.m_strides = nullptr;
}

/*
Vertex_layout_description::Vertex_layout_description(const Vertex_layout_description& a, const Vertex_layout_description& b)
{
	m_num_elements = a.m_num_elements + b.m_num_elements;
	m_elements = new Element[m_num_elements];

	for (uint32_t i = 0; i < a.m_num_elements; ++i)
	{
		m_elements[i] = a.m_elements[i];
	}

	for (uint32_t i = 0; i < b.m_num_elements; ++i)
	{
		m_elements[i + a.m_num_elements] = b.m_elements[i];
		m_elements[i + a.m_num_elements].m_slot += a.m_num_streams;
	}

	m_num_streams = a.m_num_streams + b.m_num_streams;
	m_strides    = new uint32_t[m_num_streams];

	for (uint32_t i = 0; i < a.m_num_streams; ++i)
	{
		m_strides[i] = a.m_strides[i];
	}

	for (uint32_t i = 0; i < b.m_num_streams; ++i)
	{
		m_strides[i + a.m_num_streams] = b.m_strides[i];
	}
}*/

Vertex_layout_description::~Vertex_layout_description()
{
	delete [] m_elements;
	delete [] m_strides;
}

uint32_t Vertex_layout_description::get_num_elements() const
{
	return m_num_elements;
}

const Vertex_layout_description::Element& Vertex_layout_description::operator[](uint32_t index) const
{
	return m_elements[index];
}

const Vertex_layout_description::Element* Vertex_layout_description::get_elements() const
{
	return m_elements;
}

uint32_t Vertex_layout_description::get_vertex_size() const
{
	return m_vertex_size;
}

uint32_t* Vertex_layout_description::get_strides() const
{
	return m_strides;
}

uint32_t Vertex_layout_description::get_num_streams() const
{
	return m_num_streams;
}

bool Vertex_layout_description::consists_of(uint32_t num_elements, const Element elements[]) const
{
	if (m_num_elements != num_elements)
	{
		return false;
	}

	for (uint32_t i = 0; i < m_num_elements; ++i)
	{
		if (m_elements[i] != elements[i])
		{
			return false;
		}
	}

	return true;
}

bool Vertex_layout_description::operator==(const Vertex_layout_description& description) const
{
	return consists_of(description.m_num_elements, description.m_elements);
}

bool Vertex_layout_description::operator!=(const Vertex_layout_description& description) const
{
	return !((*this) == description);
}

Vertex_layout_description::Element::Element() 
	: semantic_index(0), format(Data_format::Unknown), slot(0), byte_offset(0), slot_class(Classification::Per_vertex_data), instance_step_rate(0)
{}

Vertex_layout_description::Element::Element(const char* semantic_name, uint32_t semantic_index, Data_format::Value format, uint32_t slot, uint32_t byte_offset)
	: semantic_name(semantic_name), semantic_index(semantic_index), format(format), slot(slot),
	  byte_offset(byte_offset), slot_class(Classification::Per_vertex_data), instance_step_rate(0)
{}

Vertex_layout_description::Element::Element(const char* semantic_name, uint32_t semantic_index, Data_format::Value format, Classification::Value classification,
	                                        uint32_t step_rate, uint32_t slot, uint32_t byte_offset)
	: semantic_name(semantic_name), semantic_index(semantic_index), format(format), slot(slot),
	  byte_offset(byte_offset), slot_class(classification), instance_step_rate(step_rate)
{}

bool Vertex_layout_description::Element::operator==(const Element& element) const
{
	return boost::iequals(semantic_name, element.semantic_name)
		&& semantic_index == element.semantic_index
		&& format == element.format
		&& slot == element.slot
		&& byte_offset == element.byte_offset
		&& slot_class == element.slot_class
		&& instance_step_rate == element.instance_step_rate;
}

bool Vertex_layout_description::Element::operator!=(const Element& element) const
{
	return !((*this) == element);
}

}

std::istream& operator>>(std::istream& stream, rendering::Vertex_layout_description::Element& element)
{
	// extract name
	element.semantic_name = file::read_string(stream);

	// the rest
	stream.read((char*)&element.semantic_index, sizeof(uint32_t));
	stream.read((char*)&element.format, sizeof(uint32_t));
	stream.read((char*)&element.slot, sizeof(uint32_t));
	stream.read((char*)&element.byte_offset, sizeof(uint32_t));
	stream.read((char*)&element.slot_class, sizeof(uint32_t));
	stream.read((char*)&element.instance_step_rate, sizeof(uint32_t));

	return stream;
}
