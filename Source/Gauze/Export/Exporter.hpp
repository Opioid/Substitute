#pragma once

#include "Image_storage/Image_writer.hpp"
#include "Image_storage/Storage_options.hpp"
#include "Math/Vector.hpp"
#include <cstdint>

struct Export_item;
struct Export_set;

class Cached_image;

template<typename T> class Image_buffer;

class Exporter
{

public:

	Exporter();

	void set_target_directory(const std::string& directory, const std::string& materials_folder, const std::string& textures_folder);
	void set_container(Storage_options::Container container);

	bool write(const std::string& name, const Export_set& export_set);

private:

	bool write_color(const std::string& name, const Export_set& export_set);

	bool write_normals(const std::string& name, const Export_item& export_item);

	bool write_surface(const std::string& name, const Export_set& export_set);

	static void bake_channel(Image_buffer<rendering::Color4>& destination, uint32_t channel, const Export_item& source);

	static uint2 calculate_common_dimensions(const Cached_image* a, const Cached_image* b,
											 const Cached_image* c);

	std::string textures_target_directory_;

	Storage_options::Container container_;

	Image_writer image_writer_;
};
