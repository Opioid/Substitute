#pragma once

#include "Rendering/Scene_rendering/Scene_renderer.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Matrix.hpp"
#include "Rendering/Color.hpp"

struct AABB;
struct Sphere;
class Frustum;

namespace scene
{

class Scene;
class AABB_tree;

}

namespace rendering
{

class Bounding_renderer : public Scene_renderer
{

public:

	Bounding_renderer(Rendering_tool& rendering_tool);
	virtual ~Bounding_renderer();

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void render(const scene::Scene& scene, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	void render(const scene::AABB_tree& tree, const Frustum& frustum);
	void render(const AABB& aabb);
	void render(const Sphere& sphere);
	void render(const Frustum& frustum);

	bool create_render_states();

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	struct Change_per_camera
	{
		float4x4 view_projection;
	};

	Constant_buffer_updater<Change_per_camera> change_per_camera_;

	struct Change_per_object
	{
		float4x4 world;	// should actually be identity in this case
	};

	Constant_buffer_updater<Change_per_object> change_per_object_;

	struct Change_per_color
	{
		Color3 color;
	};

	Constant_buffer_updater<Change_per_color> change_per_color_;

	Handle<Vertex_buffer> vertex_buffer_;
	float3* vertices_;

	Handle<Rasterizer_state>    rasterizer_state_;
	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state>         blend_state_;
};

}

