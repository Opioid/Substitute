#pragma once

class asIScriptContext;

namespace scripting
{

class Script_engine;

class Script_context
{

public:

	Script_context(asIScriptContext* context);
	~Script_context();

// private:

	asIScriptContext* context_;

	friend Script_engine;
};

}
