#include "Application.hpp"
#include "Configuration.hpp"
#include "Scripting/Scripting.hpp"
#include "Scripting/Script_object_wrapper.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Baking/Light_baker.hpp"
#include "Scene/Actor.hpp"
#include "Scene/Static_prop.hpp"
#include "Scene/Transformation_interpolator.hpp"
#include "Scene/Light/Irradiance_volume.hpp"
#include "Scene/Light/Light_probe.hpp"
#include "Gui/Gui_input.hpp"
#include "Math/Vector.inl"
#include "Logging/Logging.hpp"
#include "String/String.hpp"
#include <iostream>

Application::Application() :
	request_close_(false),
	simulation_frequency_(1.0 / 60.0), time_slice_(static_cast<float>(simulation_frequency_)),
	mode_(Mode::Simulation),
	script_tool_(logging::server), scripter_(script_tool_),
	texture_provider_(rendering_tool_), effect_provider_(rendering_tool_), model_provider_(rendering_tool_),
	renderer_(rendering_tool_), printer_(rendering_tool_), gui_(logging::server, script_tool_),
	scene_loader_(scene_, scripter_, rendering_tool_, resource_manager_)
{}

Application::~Application()
{}

bool Application::run(const std::string& name, const uint2& dimensions, bool windowed)
{
	if (!init(name, dimensions, windowed))
	{
		return false;
	}

	double accumulator = simulation_frequency_;

	while (!request_close_)
	{
        client_.update();

		const platform::Message message = client_.query_message();

		if (platform::Message::Quit == message)
		{
			break;
		}
		else if (platform::Message::Nothing == message)
		{
			update();

			accumulator += fps_counter_.frame_time();

			while (accumulator >= simulation_frequency_)
			{
				/*
				previousState = currentState;
				integrate( currentState, t, dt );
				t += dt;
				*/

				scene_.on_tick(time_slice_);

				scripter_.execute_on_tick(time_slice_);

				on_tick(time_slice_);

				accumulator -= simulation_frequency_;
			}

			scene_.update(static_cast<float>(accumulator / simulation_frequency_));

			on_render();

			//---
			controls_.cleanup_update();
			//--
		}
	}

	release();

	return true;
}

double Application::simulation_frequency() const
{
	return simulation_frequency_;
}

void Application::set_simulation_frequency(double frequency)
{
	simulation_frequency_ = frequency;
}

void Application::request_close()
{
	request_close_ = true;
}

platform::Client& Application::client()
{
	return client_;
}

const std::string& Application::name() const
{
	return name_;
}

const timing::Fps_counter& Application::fps_counter() const
{
	return fps_counter_;
}

scripting::Script_tool& Application::script_tool()
{
	return script_tool_;
}

Control_mappings_interface& Application::control_mappings()
{
	return controls_.mappings();
}

Resource_manager& Application::resource_manager()
{
	return resource_manager_;
}

rendering::Rendering_tool& Application::rendering_tool()
{
	return rendering_tool_;
}

rendering::Renderer& Application::renderer()
{
	return renderer_;
}

rendering::Printer& Application::printer()
{
	return printer_;
}

Application::Mode Application::mode() const
{
	return mode_;
}

void Application::set_mode(Mode mode)
{
	mode_ = mode;
}

gui::Gui& Application::get_gui()
{
	return gui_;
}

bool Application::toggle_gui()
{
    bool gui_active = gui_.toggle_active();

    client_.show_system_cursor(gui_active);

    return gui_active;
}

const scene::Scene& Application::scene() const
{
	return scene_;
}

scene::Scene& Application::scene()
{
	return scene_;
}

bool Application::load_scene(const std::string& name)
{
	bool result = scene_loader_.load(name);

    scene_.compile();

    scene_.update(0.f);

    if (result)
    {
		renderer_.light_baker()->bake(scene_, resource_manager_);
    }

	on_scene_loaded();

	scripter_.execute_on_scene_loaded();

	resource_manager_.cleanup();

	return result;
}

const scene::Editor& Application::editor() const
{
	return editor_;
}

bool Application::on_pre_init()
{
	return true;
}

bool Application::on_post_init()
{
	return true;
}

void Application::on_scene_loaded()
{}

void Application::on_tick(float /*time_slice*/)
{}

void Application::on_input_signal(const platform::Input_signal& signal)
{
	controls_.on_input_signal(signal);
}

void Application::on_render()
{
	renderer_.render(*this, 0.f);

	printer_.begin();

	on_render_gui(printer_);

    if (gui_.is_active())
	{
		printer_.flush();
		gui_.render(printer_);
	}

	printer_.end();

	rendering_tool_.present();
}

void Application::on_render_gui(rendering::Printer& /*printer*/)
{}

bool Application::init(const std::string& name, const uint2& dimensions, bool windowed)
{
	name_ = name;

	if (!resource_manager_.virtual_file_system().is_valid())
	{
		return false;
	}

	if (!logging::init(*this))
	{
		return false;
	}

	if (!scripting::init(*this))
	{
		return false;
	}

	if (!on_pre_init())
	{
		return false;
	}

	if (!scripting::load_console_script("../Config.txt"))
	{
		configuration::client_dimensions = dimensions;
		configuration::client_windowed = windowed;
		configuration::virtual_dimensions = dimensions;
	}

	if (!client_.create(name, configuration::client_dimensions, *this))
	{
		return false;
	}

	if (!rendering_tool_.init(configuration::client_dimensions, configuration::virtual_dimensions, configuration::client_windowed, configuration::v_sync, client_))
	{
		return false;
	}

	resource_manager_.virtual_file_system().mount("../..");

	resource_manager_.register_resource_provider<rendering::Shader_resource_view>(&texture_provider_);

	effect_provider_.load_constant_buffer_classes("Effects/Constant_buffer_classes.effect");
	resource_manager_.register_resource_provider<rendering::Effect>(&effect_provider_);

	resource_manager_.register_resource_provider<scene::Model>(&model_provider_);

	resource_manager_.register_resource_provider<scene::Material>(&material_provider_);

	if (!renderer_.init(*this, resource_manager_, configuration::fxaa))
	{
		return false;
	}

	if (!printer_.init(resource_manager_))
	{
		return false;
	}

	if (!gui_.init(resource_manager_, printer_))
	{
		return false;
	}

	if (!on_post_init())
	{
		return false;
	}

	return true;
}

void Application::release()
{
	scene_.release();

	renderer_.release();

	resource_manager_.release();

	rendering_tool_.release();

	scripter_.release();

	script_tool_.release();

	logging::release();
}

void Application::update()
{
	fps_counter_.update();

	float frame_time = static_cast<float>(fps_counter_.frame_time());

	controls_.update(frame_time);

	cursor_.update(client_.relative_cursor_position());

    if (gui_.is_active())
	{
		gui_.update(frame_time);

		gui_.update(gui::Gui_input(controls_.keyboard_state(), controls_.mouse_state(), controls_.signal_stream(), cursor_, float2(rendering_tool_.dimensions())));
	}

	if (Mode::Edit == mode_)
	{
		Ray3 ray = cursor_.get_ray(scene_.camera(), rendering_tool_.dimensions());
		editor_.update(scene_, ray, scene_.camera(), controls_.mouse_state());
	}
//	client_.set_title(std::to_string(fps_counter_.fps()));
}

std::string Application::build_info()
{
#ifdef __gnu_linux__

	#if defined(__LP64__) || defined(_LP64)

		#ifdef _DEBUG
			return "Linux64 (Debug)";
		#else
			return "Linux64 (Release)";
		#endif

	#else

		#ifdef _DEBUG
			return "Linux32 (Debug)";
		#else
			return "Linux32 (Release)";
		#endif

	#endif

#elif _WIN32

	#if _WIN64

		#ifdef _DEBUG
			return "Win64 (Debug)";
		#else
			return "Win64 (Release)";
		#endif

	#else

		#ifdef _DEBUG
            return "Win32 (Debug)";
		#else
            return "Win32 (Release)";
		#endif

	#endif

#else

	return "Unknown";

#endif
}
