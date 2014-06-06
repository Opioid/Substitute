#pragma once

#include "Rendering/Texture_description.hpp"

namespace rendering
{

class Texture_data_adapter
{

public:

	Texture_data_adapter(const Texture_description& description);
	virtual ~Texture_data_adapter();

	const Texture_description& description() const;

	uint32_t num_images() const;

	virtual bool query_image(Texture_description::Data& data, uint32_t layer, uint32_t face, uint32_t level) const = 0;

protected:

	Texture_description description_;
};

class Generic_texture_data_adapter : public Texture_data_adapter
{

public:

	Generic_texture_data_adapter(const Texture_description& description);
	Generic_texture_data_adapter(const Texture_description& description, Texture_description::Data* images);
	virtual ~Generic_texture_data_adapter();

	virtual bool query_image(Texture_description::Data& data, uint32_t layer, uint32_t face, uint32_t level) const;

private:

	uint32_t image_index(uint32_t layer, uint32_t face, uint32_t level) const;

	Texture_description::Data* images_;
	const bool owns_memory_;
};

}


