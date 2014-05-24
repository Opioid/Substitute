#include "scripting_Rendering.hpp"
#include "scripting_Common.hpp"
#include "Script_tool.hpp"
#include "Application/Application.hpp"
//#include "../Rendering/Scene_rendering/Main_scene_renderer.hpp"
#include "Logging/Logging.hpp"
#include "String/String.hpp"
#include <angelscript.h>

namespace scripting
{
	std::string render_api_info();
	void enable_analytical_lighting(bool enable);
	void enable_image_based_lighting(bool enable);

	bool init_rendering(Script_tool& tool)
	{
		const Script_engine& engine = tool.engine();

		engine.set_default_namespace("rendering");

		engine.register_function("string api_info()", asFUNCTION(render_api_info));

		engine.register_function("void enable_analytical_lighting(bool)", asFUNCTION(enable_analytical_lighting));
		engine.register_function("void enable_image_based_lighting(bool)", asFUNCTION(enable_image_based_lighting));
/*
		engine.register_function("void enable_ambient_diffuse_light(bool)", asFUNCTION(enable_ambient_diffuse_light));
		engine.register_function("void enable_ambient_specular_light(bool)", asFUNCTION(enable_ambient_specular_light));
		engine.register_function("void enable_direct_light(bool)", asFUNCTION(enable_direct_light));
		engine.register_function("void enable_emissive_light(bool)", asFUNCTION(enable_emissive_light));
		engine.register_function("void enable_sky(bool)", asFUNCTION(enable_sky));
		engine.register_function("void render_actors(bool)", asFUNCTION(render_actors));
		engine.register_function("void render_irradiance_volume(bool)", asFUNCTION(render_irradiance_volume));
*/
		return true;
	}

	std::string render_api_info()
	{
		return app->rendering_tool().device().get_api_info();
	}

	void enable_analytical_lighting(bool enable)
	{
		app->renderer().set_render_analytical_lighting(enable);
	}

	void enable_image_based_lighting(bool enable)
	{
		app->renderer().set_render_image_based_lighting(enable);
	}

/*
	void enable_ambient_diffuse_light(bool enable)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_disable_ambient_diffuse_light(!enable);
	}

	void enable_ambient_specular_light(bool enable)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_disable_ambient_specular_light(!enable);
	}

	void enable_direct_light(bool enable)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_disable_direct_light(!enable);
	}

	void enable_emissive_light(bool enable)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_disable_emissive_light(!enable);
	}

	void enable_sky(bool enable)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_disable_sky(!enable);
	}

	void render_actors(bool render)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_hide_actors(!render);
	}

	void render_irradiance_volume(bool render)
	{
		app->getRenderer().get_scene_rendering_utilities().get_flags().set_render_irradiance_volume(render);
	}
	*/
}
