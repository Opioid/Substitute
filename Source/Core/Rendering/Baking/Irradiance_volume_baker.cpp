#include "Irradiance_volume_baker.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Environment_map_renderer.hpp"
#include "Ambient_cube.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Texture_transfer.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Storage/Texture_storage_save.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "Rendering/Scene_rendering/Main_scene_renderer.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Texture_provider.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Light/Irradiance_volume.hpp"
#include "Logging/Logging.hpp"
#include "String/String.hpp"
#include "File/File.hpp"

#include <iostream>

namespace rendering
{

namespace baking
{

Irradiance_volume_baker::Irradiance_volume_baker(Rendering_tool& rendering_tool, const uint2& face_dimensions) :
	rendering_tool_(rendering_tool),
	ambient_cube_integrator_(face_dimensions),
	face_dimensions_(face_dimensions),
	batch_size_(8)
{}

Irradiance_volume_baker::~Irradiance_volume_baker()
{}

bool Irradiance_volume_baker::load_cached_data(scene::Scene& scene, Resource_manager& resource_manager)
{
	auto& file_system = resource_manager.virtual_file_system();

	uint64_t scene_last_modified = file_system.file_last_modified(scene.name());

	std::string cache_load_name_template = get_cache_load_name_template(scene.name()) + "_irradiance_volume_";

	Flags flags;
	flags.set(rendering::Texture_provider::Options::Texture_3D, true);

	uint32_t i = 0;
	for (auto v : scene.get_irradiance_volumes())
	{
		for (uint32_t j = 0; j < 5; ++j)
		{
			std::string cache_load_name = cache_load_name_template + std::to_string(i) + "_" + std::to_string(j) + ".sui";

			uint64_t cache_last_modified = file_system.file_last_modified(cache_load_name);

			if (scene_last_modified > cache_last_modified)
			{
				return false;
			}

			Handle<Shader_resource_view> irradiance_volume = resource_manager.load<Shader_resource_view>(cache_load_name, flags);

			if (irradiance_volume)
			{
				v->set_texture(irradiance_volume, j);
			}
			else
			{
				return false;
			}
		}

		++i;
	}

	return true;
}

void Irradiance_volume_baker::bake(scene::Scene& scene, Environment_map_renderer& environment_map_renderer, Resource_manager& resource_manager, uint32_t pass)
{
	environment_map_renderer.configure_for_flattened_cube_map_batch(face_dimensions_);

	target_dimensions_ = environment_map_renderer.target_dimensions();

	ambient_cube_integrator_.set_pitch(target_dimensions_.x);

	Rendering_context::Rendering_options options;
	options.set(Rendering_context::Options::Render_actors, false);
	options.set(Rendering_context::Options::Render_static_geometry, true);
	options.set(Rendering_context::Options::Render_surrounding, true);
	options.set(Rendering_context::Options::Render_analytical_lighting, true);
	options.set(Rendering_context::Options::Render_image_based_lighting, pass > 0);
	options.set(Rendering_context::Options::Render_emissive_lighting, true);
	options.set(Rendering_context::Options::Face_culling, true);

	std::string cache_save_name_template;

	bool cache = pass > 0;

	if (cache)
	{
		cache_save_name_template = get_cache_save_name_template(scene.name(), resource_manager) + "_irradiance_volume_";
	}

	uint32_t i = 0;
	for (auto v : scene.get_irradiance_volumes())
	{
		bake(*v, scene, environment_map_renderer, options, cache, cache ? cache_save_name_template + std::to_string(i) : "");
		++i;
	}
}

void Irradiance_volume_baker::bake(scene::Irradiance_volume& volume, const scene::Scene& scene, Environment_map_renderer& environment_map_renderer, const Rendering_context::Rendering_options& options,
								   bool cache, const std::string& cache_template)
{
	Rendering_device& device = rendering_tool_.device();

	std::vector<Ambient_cube> ambient_cubes(volume.get_num_probes());

	Texture_description texture_description;
	texture_description.type = rendering::Texture_description::Type::Texture_2D;
	texture_description.format = rendering::Data_format::R16G16B16A16_Float;
	texture_description.dimensions = uint3(target_dimensions_, 0);
	texture_description.num_mip_levels = 1;

	Handle<Texture_transfer> texture_transfer = device.create_texture_transfer(texture_description);

	rendering::Texture_description::Data data;
	data.dimensions = texture_description.dimensions;
	data.num_bytes = texture_description.dimensions.x * texture_description.dimensions.y * Data_format::num_bytes_per_block(texture_description.format);

//	rendering::Generic_texture_data_adapter adapter(texture_description, &data);

	uint32_t batch_index = 0;
	uint32_t last_offset = 0;
	for (uint32_t i = 0; i < volume.get_num_probes(); ++i)
	{
		if (0 == batch_index)
		{
			environment_map_renderer.start_batch(options);
		}

		environment_map_renderer.render(scene, volume.get_probe_position(i), batch_index);

		if (++batch_index == batch_size_ || i == volume.get_num_probes() - 1)
		{
			Handle<Framebuffer>& framebuffer = environment_map_renderer.framebuffer();

			Handle<Render_tarview>& tarview = environment_map_renderer.color_target()->render_tarview(0);

			framebuffer->set_render_targets(tarview);
			device.set_framebuffer(framebuffer);
		//	glReadPixels(0, 0, dimensions.x, dimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, data.buffer);

			device.copy(texture_transfer, tarview/*color_target->render_tarview()*/);
			device.map(&data.buffer, texture_transfer);

		//	glGetTextureImageEXT(color_target->render_tarview(1)->id(), GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.buffer);
		//	rendering::texture_storage::save("texture" + std::to_string(batch_count) + ".png", adapter, file::File_type::PNG);

			for (uint32_t j = 0; j < batch_index; ++j)
			{
				ambient_cube_integrator_.integrate(ambient_cubes[last_offset + j], j, data.buffer);
			}

			device.unmap(texture_transfer);

			batch_index = 0;
			last_offset = i + 1;
		}
	}

	create_volume_textures(volume, ambient_cubes, cache, cache_template);
}

void Irradiance_volume_baker::create_volume_textures(scene::Irradiance_volume& volume, const std::vector<Ambient_cube>& ambient_cubes, bool cache, const std::string& cache_template)
{
	Rendering_device& device = rendering_tool_.device();

	Texture_description texture_description;
	texture_description.type = rendering::Texture_description::Type::Texture_3D;
	texture_description.format = rendering::Data_format::R8G8B8A8_UNorm;
	texture_description.dimensions = volume.get_resolution();
	texture_description.num_mip_levels = 1;

	Generic_texture_data_adapter volume_texture_adapter(texture_description);

	Texture_description::Data volume_data;
	volume_texture_adapter.get_level(volume_data, 0);

	Color4c* colors = reinterpret_cast<Color4c*>(volume_data.buffer);

	{
		for (size_t i = 0; i < ambient_cubes.size(); ++i)
		{
			colors[i] = Color4c(static_cast<unsigned char>(std::min(ambient_cubes[i].colors[0].r, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[0].g, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[0].b, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[1].r, 1.f) * 255.f));
		}

		Handle<Texture> texture = device.create_texture_3D(volume_texture_adapter);
		volume.set_texture(device.create_shader_resource_view(texture, ""), 0);

		if (cache)
		{
			std::string cache_name = cache_template + "_0.sui";
			rendering::texture_storage::save(cache_name, volume_texture_adapter, file::File_type::SUI);
		}
	}

	{
		for (size_t i = 0; i < ambient_cubes.size(); ++i)
		{
			colors[i] = Color4c(static_cast<unsigned char>(std::min(ambient_cubes[i].colors[1].g, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[1].b, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[2].r, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[2].g, 1.f) * 255.f));
		}

		Handle<Texture> texture = device.create_texture_3D(volume_texture_adapter);
		volume.set_texture(device.create_shader_resource_view(texture, ""), 1);

		if (cache)
		{
			std::string cache_name = cache_template + "_1.sui";
			rendering::texture_storage::save(cache_name, volume_texture_adapter, file::File_type::SUI);
		}
	}

	{
		for (size_t i = 0; i < ambient_cubes.size(); ++i)
		{
			colors[i] = Color4c(static_cast<unsigned char>(std::min(ambient_cubes[i].colors[2].b, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[3].r, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[3].g, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[3].b, 1.f) * 255.f));
		}

		Handle<Texture> texture = device.create_texture_3D(volume_texture_adapter);
		volume.set_texture(device.create_shader_resource_view(texture, ""), 2);

		if (cache)
		{
			std::string cache_name = cache_template + "_2.sui";
			rendering::texture_storage::save(cache_name, volume_texture_adapter, file::File_type::SUI);
		}
	}

	{
		for (size_t i = 0; i < ambient_cubes.size(); ++i)
		{
			colors[i] = Color4c(static_cast<unsigned char>(std::min(ambient_cubes[i].colors[4].r, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[4].g, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[4].b, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[5].r, 1.f) * 255.f));
		}

		Handle<Texture> texture = device.create_texture_3D(volume_texture_adapter);
		volume.set_texture(device.create_shader_resource_view(texture, ""), 3);

		if (cache)
		{
			std::string cache_name = cache_template + "_3.sui";
			rendering::texture_storage::save(cache_name, volume_texture_adapter, file::File_type::SUI);
		}
	}

	{
		for (size_t i = 0; i < ambient_cubes.size(); ++i)
		{
			colors[i] = Color4c(static_cast<unsigned char>(std::min(ambient_cubes[i].colors[5].g, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[5].b, 1.f) * 255.f),
								255,
								255);

			if (cache)
			{
				std::string cache_name = cache_template + "_4.sui";
				rendering::texture_storage::save(cache_name, volume_texture_adapter, file::File_type::SUI);
			}
		}

		Handle<Texture> texture = device.create_texture_3D(volume_texture_adapter);
		volume.set_texture(device.create_shader_resource_view(texture, ""), 4);
	}

	/*
	for (uint32_t face = 0; face < 6; ++face)
	{
		for (size_t i = 0; i < ambient_cubes.size(); ++i)
		{
			colors[i] = Color4c(static_cast<unsigned char>(std::min(ambient_cubes[i].colors[face].r, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[face].g, 1.f) * 255.f),
								static_cast<unsigned char>(std::min(ambient_cubes[i].colors[face].b, 1.f) * 255.f),
								255);
		}

		Handle<Texture> texture = device.create_texture_3D(volume_texture_adapter);

		volume.set_texture(device.create_shader_resource_view(texture, ""), face);
	}
	*/
}

}

}
