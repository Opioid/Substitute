#include "scripting_Print.hpp"
#include "Script_tool.hpp"
#include "../Logging/Logging.hpp"
#include "String/String.hpp"
#include <angelscript.h>

namespace scripting
{
	void print(bool value);
	void print(int value);
	void print(float value);
	void print(const std::string& value);

	bool init_print(Script_tool& tool)
	{
		Script_engine const& engine = tool.engine();

		engine.register_function("void print(bool)", asFUNCTIONPR(print, (bool), void));
		engine.register_function("void print(int)", asFUNCTIONPR(print, (int), void));
		engine.register_function("void print(float)", asFUNCTIONPR(print, (float), void));
		engine.register_function("void print(const string &in)", asFUNCTIONPR(print, (const std::string&), void));

		return true;
	}

	void print(bool value)
	{
		logging::output(string::to_string(value));
	}

	void print(int value)
	{
		logging::output(std::to_string(value));
	}

	void print(float value)
	{
		logging::output(std::to_string(value));
	}

	void print(const std::string& value)
	{
		logging::output(value);	
	}
}
