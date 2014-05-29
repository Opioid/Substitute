#pragma once

#include "Node.hpp"
#include "Math/Intersection/Frustum.hpp"

namespace scene
{

class Camera : public Node
{

public:

	Camera();

	const float4x4& view() const;
	const float4x4& projection() const;
	const float4x4& view_projection() const;

	const Frustum& frustum() const;

	const float3* view_rays_vs() const;
	const float3* view_rays_ws() const;

	const float2& linear_depth_projection() const;

	float fov() const;
	float ratio() const;
	float z_near() const;
	float z_far() const;

	bool is_upside_down() const;

	float greatest_distance_to_near_plane() const;

	void update_view();
	void update_frustum();

	void set_view(const float4x4& view);
	void set_projection(float fov, float ratio, float z_near, float z_far, bool upside_down = false);

	void rotate(float x, float y, float z);
	void set_rot(float x, float y, float z);
	void set_rot(const float3x3& rot);
	void translate(const float3& v);

	float exposure() const;
	void set_exposure(float exposure);

	const float3& linear_white() const;
	void set_linear_white(const float3& linear_white);

private:

	void calculate_greatest_distance_to_near_plane();

	float4x4 view_;
	float4x4 projection_;
	float4x4 view_projection_;

	Frustum frustum_;

	float3 view_rays_vs_[3];
	float3 view_rays_ws_[3];

	float2 linear_depth_projection_;

	float fov_;
	float ratio_;
	float z_near_;
	float z_far_;
	bool  upside_down_;

	float greatest_distance_to_near_plane_;

	float3x3 rot_mat_;
	float3   rot_;

	float  exposure_;
	float3 linear_white_;
};

}

