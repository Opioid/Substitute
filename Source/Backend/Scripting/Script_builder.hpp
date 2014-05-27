#pragma once

#include "scriptbuilder/scriptbuilder.h"
#include <string>
#include <set>

namespace scripting
{

class Script_engine;

class Script_builder
{

public:

	bool start_new_module(const std::string& name, const Script_engine& engine);

	bool compile_module();

	bool add_section(const std::string& name, const std::string& source);

private:

	CScriptBuilder script_builder_;
};

}
