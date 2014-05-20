#pragma once

#include <string>

struct Model;

class Exporter
{

public:

	virtual bool write(const std::string& name, const Model&  model) const = 0;
};
