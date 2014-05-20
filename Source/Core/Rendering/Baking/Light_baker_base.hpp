#pragma once

#include <string>

class Resource_manager;

namespace rendering
{

namespace baking
{

class Light_baker_base
{

protected:

	static std::string get_cache_load_name_template(const std::string& scene_name);
	static std::string get_cache_save_name_template(const std::string& scene_name, Resource_manager& resource_manager);

};

}

}
