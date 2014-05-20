#pragma once

#include "Exporter.hpp"

class Exporter_json : public Exporter
{

public:

	virtual bool write(const std::string& name, const Model&  model) const;
};
