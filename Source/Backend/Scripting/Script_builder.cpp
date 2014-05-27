#include "Script_builder.hpp"
#include "Script_engine.hpp"

namespace scripting
{

bool Script_builder::start_new_module(const std::string& name, const Script_engine& engine)
{
    return script_builder_.StartNewModule(engine.engine_, name.c_str()) >= 0;
}

bool Script_builder::compile_module()
{
    return script_builder_.BuildModule() >= 0;
}

bool Script_builder::add_section(const std::string& name, const std::string& source)
{
    return script_builder_.AddSectionFromMemory(name.c_str(), source.data(), static_cast<uint32_t>(source.size())) >= 0;
}

}
