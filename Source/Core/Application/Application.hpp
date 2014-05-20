#pragma once

#include "Platform/Client.hpp"
#include "Timing/Fps_counter.hpp"
#include "Cursor.hpp"
#include "../Controls/Controls.hpp"
#include "Scripting/Script_tool.hpp"
#include "Resources/Resource_manager.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Printer.hpp"
#include "Rendering/Texture_provider.hpp"
#include "Rendering/Effect/Effect_provider.hpp"
#include "Gui/Gui.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Model_provider.hpp"
#include "Scene/Material_provider.hpp"
#include "Scene/Scene_loader.hpp"
#include "Scene/Editor/Editor.hpp"

class Application : protected platform::Input_sink
{

public:

	enum class Mode
	{
		Game,
		Simulation,
		Edit
	};

	Application();
	virtual ~Application();

	bool run(const std::string& name, const uint2& size, bool windowed);

	void request_close();

	platform::Client& get_client();

	const std::string& get_name() const;

	const timing::Fps_counter& get_fps_counter() const;

	scripting::Script_tool& get_script_tool();

	Control_mappings_interface& get_control_mappings();

	Resource_manager& get_resource_manager();

	rendering::Rendering_tool& get_rendering_tool();
	rendering::Renderer& get_renderer();
	rendering::Printer& get_printer();

	Mode get_mode() const;
	void set_mode(Mode mode);

	gui::Gui& get_gui();

	bool toggle_gui();

	const scene::Scene& get_scene() const;
	scene::Scene& get_scene();

	bool load_scene(const std::string& name);

	const scene::Editor& get_editor() const;

protected:

	virtual bool on_pre_init();

	virtual bool on_post_init();

	virtual void on_load_scene();

	virtual void on_tick();

	virtual void on_input_signal(const platform::Input_signal& signal);

	virtual void on_render();

	virtual void on_render_gui(rendering::Printer& printer);

private:

	bool init(const std::string& name, const uint2& size, bool windowed);

	void release();

	void update();

	platform::Client client_;

	bool request_close_;

	std::string name_;

	timing::Fps_counter fps_counter_;

	double simulation_frequency_;

	Mode mode_;

	scripting::Script_tool script_tool_;

	Controls controls_;

	Resource_manager resource_manager_;

	rendering::Rendering_tool rendering_tool_;

	rendering::Texture_provider texture_provider_;
	rendering::Effect_provider  effect_provider_;
	scene::Model_provider       model_provider_;
	scene::Material_provider    material_provider_;

	rendering::Renderer renderer_;
	rendering::Printer  printer_;

	Cursor   cursor_;
	gui::Gui gui_;

	scene::Scene		scene_;
	scene::Scene_loader scene_provider_;
	scene::Editor		editor_;

public:

	static std::string get_build_info();
};