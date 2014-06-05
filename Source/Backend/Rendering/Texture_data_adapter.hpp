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

	uint32_t num_blocks() const;

	virtual bool get_level(Texture_description::Data& data, uint32_t level) const = 0;
	virtual bool get_image(Texture_description::Data& data, uint32_t level, uint32_t layer) const = 0;

protected:

	Texture_description description_;
};

class Generic_texture_data_adapter : public Texture_data_adapter
{

public:

	Generic_texture_data_adapter(const Texture_description& description);
	Generic_texture_data_adapter(const Texture_description& description, Texture_description::Data* blocks, bool owns_memory = false);
	virtual ~Generic_texture_data_adapter();

	virtual bool get_level(Texture_description::Data& data, uint32_t level) const;
	virtual bool get_image(Texture_description::Data& data, uint32_t level, uint32_t layer) const;

private:

	Texture_description::Data* blocks_;
	const bool owns_memory_;
};

}


