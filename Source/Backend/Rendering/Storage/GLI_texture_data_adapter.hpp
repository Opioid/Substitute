#pragma once

#include "Rendering/Texture_data_adapter.hpp"
#include <gli/gli.hpp>

namespace rendering
{

class GLI_texture_data_adapter : public Texture_data_adapter
{

public:

	GLI_texture_data_adapter(const Texture_description& description, const gli::storage& storage);
	virtual ~GLI_texture_data_adapter();

	virtual bool get_level(Texture_description::Data& data, uint32_t level) const;
	virtual bool get_image(Texture_description::Data& data, uint32_t level, uint32_t layer) const;

	static Data_format::Value map(gli::format gli_format);

private:

	gli::storage storage_;
};

}


