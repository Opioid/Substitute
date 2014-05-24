#include "Vertex_layout_description.hpp"
#include "File/Stream.hpp"
#include <algorithm>
#include <cstring>
#include <boost/algorithm/string.hpp>   

namespace rendering
{

Vertex_layout_description::Vertex_layout_description(uint32_t num_elements, const Element elements[]) : num_elements_(num_elements), vertex_size_(0) , num_streams_(0)
{
	elements_ = new Element[num_elements_];

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		elements_[i] = elements[i];
		num_streams_ = std::max(num_streams_, elements_[i].slot);
	}

	++num_streams_;
	strides_ = new uint32_t[num_streams_];

	std::fill_n(strides_, num_streams_, 0);

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		if (0xFFFFFFFF == elements_[i].byte_offset)
		{
			elements_[i].byte_offset = strides_[elements_[i].slot];
		}
		else
		{
			strides_[elements_[i].slot] = elements_[i].byte_offset;
		}

		strides_[elements_[i].slot] += uint32_t(Data_format::num_bytes_per_block(elements_[i].format));
	}

	for (uint32_t i = 0; i < num_streams_; ++i)
	{
		vertex_size_ += strides_[i];
	}
}

Vertex_layout_description::Vertex_layout_description(const Vertex_layout_description& other) :
	num_elements_(other.num_elements_), elements_(new Element[other.num_elements_]), vertex_size_(other.vertex_size_),
	num_streams_(other.num_streams_), strides_(new uint32_t[other.num_streams_])
{
	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		elements_[i] = other.elements_[i];
	}

	for (uint32_t i = 0; i < num_streams_; ++i)
	{
		strides_[i] = other.strides_[i];
	}
}

Vertex_layout_description::Vertex_layout_description(Vertex_layout_description&& other)
	: num_elements_(other.num_elements_), elements_(other.elements_), vertex_size_(other.vertex_size_),
	  num_streams_(other.num_streams_), strides_(other.strides_)
{
	other.num_elements_ = 0;
	other.elements_ = nullptr;

	other.num_streams_ = 0;
	other.strides_ = nullptr;
}

/*
Vertex_layout_description::Vertex_layout_description(const Vertex_layout_description& a, const Vertex_layout_description& b)
{
	num_elements_ = a.num_elements_ + b.num_elements_;
	elements_ = new Element[num_elements_];

	for (uint32_t i = 0; i < a.num_elements_; ++i)
	{
		elements_[i] = a.elements_[i];
	}

	for (uint32_t i = 0; i < b.num_elements_; ++i)
	{
		elements_[i + a.num_elements_] = b.elements_[i];
		elements_[i + a.num_elements_].m_slot += a.num_streams_;
	}

	num_streams_ = a.num_streams_ + b.num_streams_;
	strides_    = new uint32_t[num_streams_];

	for (uint32_t i = 0; i < a.num_streams_; ++i)
	{
		strides_[i] = a.strides_[i];
	}

	for (uint32_t i = 0; i < b.num_streams_; ++i)
	{
		strides_[i + a.num_streams_] = b.strides_[i];
	}
}*/

Vertex_layout_description::~Vertex_layout_description()
{
	delete [] elements_;
	delete [] strides_;
}

uint32_t Vertex_layout_description::num_elements() const
{
	return num_elements_;
}

const Vertex_layout_description::Element& Vertex_layout_description::operator[](uint32_t index) const
{
	return elements_[index];
}

const Vertex_layout_description::Element* Vertex_layout_description::elements() const
{
	return elements_;
}

uint32_t Vertex_layout_description::vertex_size() const
{
	return vertex_size_;
}

uint32_t* Vertex_layout_description::strides() const
{
	return strides_;
}

uint32_t Vertex_layout_description::num_streams() const
{
	return num_streams_;
}

bool Vertex_layout_description::consists_of(uint32_t num_elements, const Element elements[]) const
{
	if (num_elements != num_elements_)
	{
		return false;
	}

	for (uint32_t i = 0; i < num_elements_; ++i)
	{
		if (elements_[i] != elements[i])
		{
			return false;
		}
	}

	return true;
}

bool Vertex_layout_description::operator==(const Vertex_layout_description& description) const
{
	return consists_of(description.num_elements_, description.elements_);
}

bool Vertex_layout_description::operator!=(const Vertex_layout_description& description) const
{
	return !((*this) == description);
}

Vertex_layout_description::Element::Element() :
	semantic_index(0), format(Data_format::Unknown), slot(0), byte_offset(0), slot_class(Classification::Per_vertex_data), instance_step_rate(0)
{}

Vertex_layout_description::Element::Element(const char* semantic_name, uint32_t semantic_index, Data_format::Value format, uint32_t slot, uint32_t byte_offset) :
	semantic_name(semantic_name), semantic_index(semantic_index), format(format), slot(slot),
	byte_offset(byte_offset), slot_class(Classification::Per_vertex_data), instance_step_rate(0)
{}

Vertex_layout_description::Element::Element(const char* semantic_name, uint32_t semantic_index, Data_format::Value format, Classification classification,
											uint32_t step_rate, uint32_t slot, uint32_t byte_offset) :
	semantic_name(semantic_name), semantic_index(semantic_index), format(format), slot(slot),
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
