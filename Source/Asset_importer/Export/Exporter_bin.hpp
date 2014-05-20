#pragma once

#include "Exporter.hpp"

class Exporter_bin : public Exporter
{

public:

	virtual bool write(const std::string& name, const Model&  model) const;
};
