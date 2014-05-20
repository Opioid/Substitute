#pragma once

#include "Rendering/Primitive_topology.hpp"
#include <string>

struct Model;

class Importer
{

public:

	Model* read(const std::string& name) const;

private:

	static rendering::Primitive_topology get_primitve_topology(const std::string& name);
};
