#pragma once

#include "../Texture_data_adapter.hpp"
#include <memory>

namespace rendering
{


class Texture_data_adapter_cube : public Texture_data_adapter
{

public:

	Texture_data_adapter_cube(const Texture_description& description, const std::shared_ptr<Texture_data_adapter> texture_data_[6]);
	virtual ~Texture_data_adapter_cube();

	virtual bool query_image(Texture_description::Data& data, uint32_t layer, uint32_t face, uint32_t level) const;

private:

	std::shared_ptr<Texture_data_adapter> texture_data_[6];
};

}


