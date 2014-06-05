#pragma once

#include "Resources/Resource_provider.hpp"
#include "Material.hpp"
#include "Parsing/Rapidjson_types.hpp"
#include <cstdint>

namespace scene
{

class Material_provider : public Resource_provider<Material>
{

public:

	Material_provider();

	Handle<Material> load(file::Input_stream& stream, Resource_manager& resource_manager, uint32_t flags = 0) const;

	Handle<Material> clone(const Handle<Material>& material, Resource_manager& resource_manager) const;

private:

	static void fill_particle_properties(Material* material, const rapidjson::Value& particle_value);

	static Material::Sampler read_sampler(const rapidjson::Value& value);

	static rendering::Color3 read_color(const rapidjson::Value& color_value);

	static std::string read_file_name(const rapidjson::Value& value);
};

}
