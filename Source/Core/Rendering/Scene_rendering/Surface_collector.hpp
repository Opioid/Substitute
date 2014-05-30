#pragma once

#include <vector>
#include <cstdint>

template<typename T> struct Vector3;
typedef Vector3<float> float3;

class Frustum;

namespace scene
{

class Scene;
class Prop;
class AABB_tree;
class AABB_node;
class Renderable;
struct Surface;

}

namespace rendering
{

struct Render_surface
{

	Render_surface(const scene::Surface* surface, uint32_t model_id, uint32_t group_id, float distance);

	bool operator<(const Render_surface& surface) const;

	uint64_t              signature;
	const scene::Surface* surface;
};

class Surface_collector
{

public:

	void collect        (const scene::Scene& scene, const float3& eye_position, const Frustum& frustum, bool collect_actors = true, bool collect_static_geometry = true);
	void collect_unified(const scene::Scene& scene, const float3& eye_position, const Frustum& frustum, bool collect_actors = true, bool collect_static_geometry = true);

	const std::vector<Render_surface>& get_surfaces() const;

private:

	void collect        (const scene::AABB_tree& tree, const float3& eye_position, const Frustum& frustum);
	void collect_unified(const scene::AABB_tree& tree, const float3& eye_position, const Frustum& frustum);

	void add        (const scene::AABB_node* node, const float3& eye_position);
	void add_unified(const scene::AABB_node* node, const float3& eye_position);

	void add        (const scene::Prop* prop, const float3& eye_position);
	void add_unified(const scene::Prop* prop, const float3& eye_position);

	std::vector<Render_surface> surfaces_;

};

}


