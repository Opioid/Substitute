#include "Camera.hpp"

namespace scene
{

Camera::Camera() : upside_down_(false), rot_mat_(float3x3::identity), rot_(float3::identity), exposure_(1.f), linear_white_(float3(11.2f, 11.2f, 11.2f))
{}

const float4x4& Camera::view() const
{
	return view_;
}

const float4x4& Camera::projection() const
{
	return projection_;
}

const float4x4& Camera::view_projection() const
{
	return view_projection_;
}

const Frustum& Camera::frustum() const
{
	return frustum_;
}

const float3* Camera::view_rays_vs() const
{
	return view_rays_vs_;
}

const float3* Camera::view_rays_ws() const
{
	return view_rays_ws_;
}

const float2& Camera::linear_depth_projection() const
{
	return linear_depth_projection_;
}

float Camera::FOV() const
{
	return fov_;
}

float Camera::ratio() const
{
	return ratio_;
}

float Camera::z_near() const
{
	return z_near_;
}

float Camera::z_far() const
{
	return z_far_;
}

bool Camera::is_upside_down() const
{
	return upside_down_;
}

float Camera::greatest_distance_to_near_plane() const
{
	return greatest_distance_to_near_plane_;
}

void Camera::update_view()
{
	float3x3 rot;
	set_rotation_y(rot, rot_.y);
	rot_mat_ *= rot;

	::set_rotation(rot, rot_mat_.x, rot_.x);
	rot_mat_ *= rot;

	::set_rotation(rot, rot_mat_.z, rot_.z);
	rot_mat_ *= rot;

	rot_ = float3::identity;

	set_local_rotation(Quaternion(rot_mat_));
	fix_world_transformation();

	set_look_at(view_, world_position(), world_position() + rot_mat_.z, rot_mat_.y);

	update_frustum();
}

void Camera::update_frustum()
{
	view_projection_ = view_ * projection_;

	frustum_.set_from_matrix(view_projection_);

	float rcp_z_far = 1.f / z_far_;

	view_rays_vs_[0] = rcp_z_far * (frustum_.points_[6] * view_);	// Left  Upper  Far
	view_rays_vs_[1] = rcp_z_far * (frustum_.points_[7] * view_);	// Right Upper  Far
	view_rays_vs_[2] = rcp_z_far * (frustum_.points_[4] * view_);	// Left  Bottom Far

	// Because we use a windowed triangle instead of a quad!
	float3 ruf = view_rays_vs_[0] + 2.f * (view_rays_vs_[1] - view_rays_vs_[0]);
	float3 lbf = view_rays_vs_[0] + 2.f * (view_rays_vs_[2] - view_rays_vs_[0]);

	view_rays_vs_[1] = ruf;
	view_rays_vs_[2] = lbf;

	// Transforming the view space rays into worldspace, but without the camera translation!
	float3x3 view = float3x3(view_);

	view_rays_ws_[0] = view * view_rays_vs_[0];	// == view_rays_vs_[0] * inverted(view);
	view_rays_ws_[1] = view * view_rays_vs_[1];
	view_rays_ws_[2] = view * view_rays_vs_[2];

	calculate_greatest_distance_to_near_plane();
}

void Camera::set_view(const float4x4& view)
{
	view_ = view;
}

void Camera::set_projection(float fov, float ratio, float z_near, float z_far, bool upside_down)
{
	fov_ = fov;
	ratio_ = ratio;
	z_near_ = z_near;
	z_far_ = z_far;
	upside_down_ = upside_down;

	set_perspective(projection_, fov, ratio, z_near, z_far, upside_down);

	linear_depth_projection_.x = z_far / (z_far - z_near);
	linear_depth_projection_.y = (-z_far * z_near) / (z_far - z_near);
}

void Camera::rotate(float x, float y, float z)
{
	rot_.x += x;
	rot_.y += y;
	rot_.z += z;
}

void Camera::set_rot(float x, float y, float z)
{
	rot_mat_ = float3x3::identity;

	rot_.x = x;
	rot_.y = y;
	rot_.z = z;
}

void Camera::set_rot(const float3x3& rot)
{
	rot_mat_ = rot;

	rot_.x = rot_.y = rot_.z = 0.f;
}

void Camera::translate(const float3& v)
{
	set_local_position(local_position() + v);
}

void Camera::calculate_greatest_distance_to_near_plane()
{
	const float a = squared_distance(world_position(), frustum_.points_[0]);
	const float b = squared_distance(world_position(), frustum_.points_[1]);
	const float c = squared_distance(world_position(), frustum_.points_[2]);
	const float d = squared_distance(world_position(), frustum_.points_[3]);

	greatest_distance_to_near_plane_ = sqrt(std::max(a, std::max(b, std::max(c, d))));
}

float Camera::exposure() const
{
	return exposure_;
}

void Camera::set_exposure(float exposure)
{
	exposure_ = exposure;
}

const float3& Camera::linear_white() const
{
	return linear_white_;
}

void Camera::set_linear_white(const float3& linear_white)
{
	linear_white_ = linear_white;
}

}
