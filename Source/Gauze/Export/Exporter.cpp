#include "Exporter.hpp"
#include "Export_item.hpp"
#include "Image/Image.hpp"
#include "Image/Image_provider.hpp"
#include "Image/Filter/To_BGRA.hpp"
#include "Image/Filter/Select_channel.hpp"
#include "Image/Filter/Invert.hpp"
#include "Image/Filter/Modulate.hpp"
#include "Image/Colorspace.hpp"
#include "Image_storage/Image_writer.hpp"
#include "Image_storage/Storage_options.hpp"
#include <algorithm>
#include <nvtt/nvtt.h>

Exporter::Exporter() : container_(Storage_options::Container::DDS)
{}

void Exporter::set_target_directory(const std::string& directory, const std::string& materials_folder, const std::string& textures_folder)
{
	textures_target_directory_ = directory;

	if (textures_target_directory_.back() != '/' && textures_target_directory_.back() != '\\')
	{
		textures_target_directory_.append("/");
	}

	if (!textures_folder.empty())
	{
		textures_target_directory_.append(textures_folder + "/");
	}
}

void Exporter::set_container(Storage_options::Container container)
{
	container_ = container;
}

bool Exporter::write(const std::string &name, const Export_set &export_set)
{
	if (name.empty())
	{
		return false;
	}

	std::string file_name_prefix = textures_target_directory_ + name;

	write_color(file_name_prefix, export_set);

	write_normals(file_name_prefix, export_set.normals);

	write_surface(file_name_prefix, export_set);

	return true;
}

bool Exporter::write_color(const std::string& name, const Export_set& export_set)
{
	Image<rendering::Color4> color_image;

	if (Export_item::Selection::Constant != export_set.color.selection)
	{
		const Image_buffer<rendering::Color4>& source_buffer = export_set.color.cached_image->get_float_image_buffer();

		color_image.init(source_buffer);
	}

	// Constant opacity/transparency is currently not baked into the texture and should be set as a material constant instead
	const bool transparent = Export_item::Selection::Constant != export_set.opacity.selection
						  && Export_item::Selection::Ignore != export_set.opacity.selection;

	if (transparent)
	{
		const Image_buffer<rendering::Color4>& opacity_buffer = export_set.opacity.cached_image->get_float_image_buffer();

		if (color_image.empty())
		{
			color_image.init(opacity_buffer.get_format(), opacity_buffer.get_dimensions(), rendering::Color4(export_set.color.constant));
		}
		else if (color_image.get_level(0)->get_dimensions() != opacity_buffer.get_dimensions())
		{
			return false;
		}

		bake_channel(*color_image.get_level(0), 3, export_set.opacity);
	}

	if (color_image.empty())
	{
		return true;
	}

	const bool normal_map = false;

	Storage_options options(rendering::Data_format::BC1_UNorm_sRGB, transparent, normal_map, container_);

	image_writer_.write(name + "_color", color_image, options);

	return true;
}

bool Exporter::write_normals(const std::string& name, const Export_item& export_item)
{
	if (Export_item::Selection::Ignore == export_item.selection)
	{
		return true;
	}

	const bool transparent = false;
	const bool normal_map  = true;

	Storage_options options(rendering::Data_format::BC5_UNorm, transparent, normal_map, container_);

	image_writer_.write(name + "_normals", export_item.cached_image->get_float_image_buffer(false), options);

	return true;
}

bool Exporter::write_surface(const std::string& name, const Export_set& export_set)
{
	uint2 surface_dimensions = calculate_common_dimensions(export_set.metallic.cached_image,
														   export_set.cavity.cached_image,
														   export_set.emissive.cached_image);

	if (uint2::identity == surface_dimensions)
	{
		return false;
	}

	Image<rendering::Color4> surface_image(rendering::Data_format::R32G32B32A32_Float, surface_dimensions);

	Image_buffer<rendering::Color4>& surface_buffer = *surface_image.get_level(0);

	if (Export_item::Selection::Constant == export_set.metallic.selection)
	{
		surface_buffer.clear_channel(0, export_set.metallic.constant.x);
	}
	else
	{
		bake_channel(surface_buffer, 0, export_set.metallic);
	}

	if (Export_item::Selection::Constant == export_set.roughness.selection)
	{
		surface_buffer.clear_channel(1, export_set.roughness.constant.x);
	}
	else
	{
		bake_channel(surface_buffer, 1, export_set.roughness);
	}

	if (Export_item::Selection::Ignore == export_set.cavity.selection)
	{
		surface_buffer.clear_channel(2, 1.f);
	}
	else
	{
		bake_channel(surface_buffer, 2, export_set.cavity);
	}

	const bool transparent = false;
	const bool normal_map  = false;

	Storage_options options(rendering::Data_format::BC1_UNorm, transparent, normal_map, container_);

	image_writer_.write(name + "_surface", surface_image, options);

	return true;
}

void Exporter::bake_channel(Image_buffer<rendering::Color4>& destination, uint32_t channel, const Export_item& source)
{
	Select_channel<rendering::Color4> select_channel(channel, static_cast<uint32_t>(source.selection));

	select_channel.filter(destination, source.cached_image->get_float_image_buffer());

	if (source.inverted)
	{
		Invert_channel<rendering::Color4> invert_channel(channel);

		invert_channel.filter(destination, destination);
	}

	if (1.f != source.modulate_factor)
	{
		Modulate_channel modulate(channel, source.modulate_factor);

		modulate.filter(destination, destination);
	}
}

uint2 Exporter::calculate_common_dimensions(const Cached_image* a, const Cached_image* b,
											const Cached_image* c/*, const Image_buffer<rendering::Color4c>* d*/)
{
	uint2 a_dimensions = a ? a->get_source_image().get_level(0)->get_dimensions() : uint2::identity;
	uint2 b_dimensions = b ? b->get_source_image().get_level(0)->get_dimensions() : uint2::identity;
	uint2 c_dimensions = c ? c->get_source_image().get_level(0)->get_dimensions() : uint2::identity;
//	uint2 d_dimensions = d ? d->get_dimensions() : uint2::identity;

	if (a)
	{
		if (b)
		{
			if (a_dimensions != b_dimensions)
			{
				return uint2::identity;
			}
		}

		if (c)
		{
			if (a_dimensions != c_dimensions)
			{
				return uint2::identity;
			}
		}

		return a_dimensions;
	}
	else if (b)
	{
		if (c)
		{
			if (b_dimensions != c_dimensions)
			{
				return uint2::identity;
			}
		}

		return b_dimensions;
	}

	return c_dimensions;
}
