#pragma once

#include "Image.hpp"
#include "Rendering/Color.hpp"

class Mip_map_generator
{

public:

    static Image<rendering::Color4>* create_mip_mapped_image(const Image_buffer<rendering::Color4>& source);
};
