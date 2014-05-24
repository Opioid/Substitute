#include "scripting_Configuration.hpp"
#include "Script_tool.hpp"
#include "Application/Configuration.hpp"

namespace scripting
{

bool init_configuration(Script_tool& tool)
{
	const Script_engine& engine = tool.engine();

	engine.register_variable("uint2 client_dimensions", &configuration::client_dimensions);
	engine.register_variable("bool client_windowed", &configuration::client_windowed);
    engine.register_variable("int v_sync", &configuration::v_sync);
	engine.register_variable("uint2 virtual_dimensions", &configuration::virtual_dimensions);
	engine.register_variable("bool fxaa", &configuration::fxaa);

	return true;
}

}
