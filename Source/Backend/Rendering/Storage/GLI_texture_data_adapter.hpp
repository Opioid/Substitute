#pragma once

#include "Rendering/Texture_data_adapter.hpp"
#include <gli/gli.hpp>

namespace rendering
{

class GLI_texture_data_adapter
{

public:

	static Data_format::Value map(gli::format gli_format);
};

class GLI_texture_data_adapter_2D : public Texture_data_adapter
{

public:

	GLI_texture_data_adapter_2D(const Texture_description& description, const gli::texture2D& texture);
	virtual ~GLI_texture_data_adapter_2D();

	virtual bool get_level(Texture_description::Data& data, uint32_t level) const;
	virtual bool get_level(Texture_description::Data& data, uint32_t layer, uint32_t level) const;

private:

	gli::texture2D texture_;
};

class GLI_texture_data_adapter_cube : public Texture_data_adapter
{

public:

	GLI_texture_data_adapter_cube(const Texture_description& description, const gli::textureCube& texture);
	virtual ~GLI_texture_data_adapter_cube();

	virtual bool get_level(Texture_description::Data& data, uint32_t level) const;
	virtual bool get_level(Texture_description::Data& data, uint32_t layer, uint32_t level) const;

private:

	gli::textureCube texture_;
};

}


