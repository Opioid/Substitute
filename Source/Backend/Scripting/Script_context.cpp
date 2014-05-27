#include "Script_context.hpp"
#include <angelscript.h>

namespace scripting
{

Script_context::Script_context(asIScriptContext* context) : context_(context)
{}

Script_context::~Script_context()
{
	if (context_)
	{
		context_->Release();
	}
}

}
