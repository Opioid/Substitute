#include "Light_baker_base.hpp"
#include "Resources/Resource_manager.hpp"
#include "File/File.hpp"

namespace rendering
{

namespace baking
{

std::string Light_baker_base::get_cache_load_name_template(const std::string& scene_name)
{
	return file::name_with_prefix(file::name_without_extension(scene_name), ".");
}

std::string Light_baker_base::get_cache_save_name_template(const std::string& scene_name, Resource_manager& resource_manager)
{
	auto& file_system = resource_manager.virtual_file_system();

	std::string cache_template;
	file_system.query_resolved_name(cache_template, scene_name);

	return file::name_with_prefix(file::name_without_extension(cache_template), ".");
}

}

}
