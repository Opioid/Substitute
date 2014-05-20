#include "Script_engine.hpp"
#include <angelscript.h>
//#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring.h>
#include <scripthelper.h>

namespace scripting
{
    Script_engine::Script_engine() //: m_default_module(nullptr)
	{
        engine_ = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		
        if (engine_)
		{
            RegisterStdString(engine_);

            engine_->SetMessageCallback(asMETHOD(Script_engine, internal_message_callback), this, asCALL_THISCALL);

            default_context_ = engine_->CreateContext();
			
	#ifdef SE_USE_WATCHDOG
			m_watchdogData.m_shutDown = false;
            m_watchdogData.context_ToWatch = default_context_;
			m_watchdogData.m_tickLength = 250;

			m_watchdog.create(watchdogThread, &m_watchdogData);
	#endif	
		}
	}

	Script_engine::~Script_engine() 
	{
	//	releaseResources();
		//Seems like you should release the context before the module or risk a crash?

#ifdef SE_USE_WATCHDOG
		m_watchdogData.m_shutDown = true;
		m_watchdog.wait();
#endif

        if (default_context_)
		{
            default_context_->Release();
		}

        if (engine_)
		{
            engine_->DiscardModule(0);
            engine_->Release();
		}	
	}

	void Script_engine::release_resources()
	{
        if (default_context_)
		{
			/*
#ifdef SE_USE_WATCHDOG
			m_watchdogData.m_lock.enter();
                default_context_->Release();
                default_context_ = engine_->CreateContext();
                m_watchdogData.context_ToWatch = default_context_;
			m_watchdogData.m_lock.leave();
#elif*/
    //		default_context_->Release();
    //		default_context_ = engine_->CreateContext();
//#endif
		}

		garbage_collect();
	}

	bool Script_engine::is_valid() const
	{
        return engine_ != nullptr;
	}

	bool Script_engine::garbage_collect(int flags) const
	{
        return engine_->GarbageCollect(flags) >= 0;
	}

	bool Script_engine::execute_string(const std::string& script) const
	{
#ifdef SE_USE_WATCHDOG
		m_watchdogData.m_lock.enter();
			m_watchdogData.m_ticks = 4;
		m_watchdogData.m_lock.leave();
#endif

        int result = ExecuteString(engine_, script.c_str(), 0, default_context_);

		if (asEXECUTION_ABORTED == result)
		{
			send_message("Script_engine::execute_string(): Script execution aborted - probably timed out.", Message::Value::Error);
		}

		return result >= 0;
	}

	bool Script_engine::set_default_namespace(const std::string& namespace_name) const
	{
		return engine_->SetDefaultNamespace(namespace_name.c_str()) >= 0;
	}

	bool Script_engine::register_variable(const std::string& name, void* pointer) const
	{
        return engine_->RegisterGlobalProperty(name.c_str(), pointer) >= 0;
	}

	bool Script_engine::register_function(const std::string& name, asSFuncPtr const& function, Calling_convention::Value calling_convention) const
	{
        return engine_->RegisterGlobalFunction(name.c_str(), function, calling_convention) >= 0;
	}

	bool Script_engine::register_object_type(const std::string& name, int byte_size, int flags) const
	{
        return engine_->RegisterObjectType(name.c_str(), byte_size, flags) >= 0;
	}

	bool Script_engine::register_object_property(const std::string& object, const std::string& declaration, int byte_offset) const
	{
        return engine_->RegisterObjectProperty(object.c_str(), declaration.c_str(), byte_offset) >= 0;
	}

	bool Script_engine::register_object_behavior(const std::string& object, int behavior, const std::string& declaration, asSFuncPtr const& function, Calling_convention::Value calling_convention) const
	{
        return engine_->RegisterObjectBehaviour(object.c_str(), asEBehaviours(behavior), declaration.c_str(), function, calling_convention) >= 0;
	}

	bool Script_engine::register_object_method(const std::string& object, const std::string& declaration, asSFuncPtr const& function, Calling_convention::Value calling_convention) const
	{
        return engine_->RegisterObjectMethod(object.c_str(), declaration.c_str(), function, calling_convention) >= 0;
	}

	void Script_engine::internal_message_callback(const asSMessageInfo* msg) const
	{
		Message::Value type;

		switch (msg->type)
		{
			case asMSGTYPE_WARNING: type = Message::Value::Warning; break;
			case asMSGTYPE_ERROR:   type = Message::Value::Error; break;

			default: type = Message::Value::Information; break;
		}

		send_message(std::string(msg->message), type);
	}

	void Script_engine::send_message(const std::string& text, Message::Value type) const
	{
        if (message_callback_)
		{
            message_callback_(text, type);
		}
	}

#ifdef SE_USE_WATCHDOG
	DWORD WINAPI Script_engine::watchdogThread(void* arg)
	{
		WatchdogData *data = static_cast<WatchdogData*>(arg);

		while (true)
		{
			Sleep(data->m_tickLength);

			data->m_lock.enter();
				if (--data->m_ticks == 0 && data->context_ToWatch->GetState() == asEXECUTION_ACTIVE)
					data->context_ToWatch->Abort();
			data->m_lock.leave();

			if (data->m_shutDown) return 0;
		}

		return 0;
	}
#endif

}
