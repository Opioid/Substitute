#pragma once

#include "Rendering/Color.hpp"
#include <string>

template<typename T> class Image;

class Image_loader
{

public:

    static Image<rendering::Color4c>* load(const std::string& file_name);
};
