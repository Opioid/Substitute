#include "Storage_options.hpp"

Storage_options::Storage_options(rendering::Data_format::Value format, bool alpha, bool normal_map, Container container) :
	format_(format),
	alpha_(alpha),
	normal_map_(normal_map),
	container_(container)
{}

rendering::Data_format::Value Storage_options::get_format() const
{
	return format_;
}

bool Storage_options::has_alpha() const
{
	return alpha_;
}

bool Storage_options::is_normal_map() const
{
	return normal_map_;
}

Storage_options::Container Storage_options::get_container() const
{
	return container_;
}

std::string Storage_options::get_file_extension() const
{
	return get_file_extension(container_);
}

std::string Storage_options::get_file_extension(Container container)
{
	switch (container)
	{
	case Container::DDS:
		return "dds";
	case Container::PNG:
		return "png";
	default:
		return "unknown";
	}
}

std::string Storage_options::get_container_name(Container container)
{
	static std::string names[] = { "DDS", "PNG", "unknown" };

	size_t index = static_cast<size_t>(container);

	return names[index];
}

Storage_options::Container Storage_options::get_container_from_name(const std::string& name)
{
	size_t num_containers = static_cast<size_t>(Container::Count);

	for (size_t i = 0; i < num_containers; ++i)
	{
		Container container = static_cast<Container>(i);

		if (name == get_container_name(container))
		{
			return container;
		}
	}

	// I believe it is easiest to just default to a valid container here even if garbage was requested
	return Container::DDS;
}
