#pragma once

#include "Math/Vector.hpp"
#include "Rendering/Color.hpp"

rendering::Color4c to_color4c_alpha_one(const rendering::Color4& color);

rendering::Color4c to_color4c(const rendering::Color4& color);

rendering::Color4c to_BGRA_order(const rendering::Color4c& color);
