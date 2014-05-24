#include "Cursor.hpp"
#include "Math/Matrix.hpp"
#include "Scene/Camera.hpp"

void Cursor::update(const float2& coords)
{
	previous_coords_ = coords_;

	coords_ = coords;
}

const float2& Cursor::coord() const
{
	return coords_;
}

const float2& Cursor::previous_coord() const
{
	return previous_coords_;
}

Ray3 Cursor::get_ray(const scene::Camera& camera, const uint2& screen_size) const
{
	const float4x4& projection = camera.projection();

	// The position of the pointer on the screen in a [0, 1] range
	float x = coords_.x / float(screen_size.x);
	float y = coords_.y / float(screen_size.y);

	// Compute the vector of the pick ray in screen space
	float3 v;
	v.x =  (2.f * x - 1.f) / projection.m00;
	v.y = -(2.f * y - 1.f) / projection.m11;
	v.z =  1.f;

	// Get the inverse view matrix
	float4x4 view = invert(camera.view());

	// Transform the screen space pick ray into 3D space
	Ray3 ray;
	ray.dir.x = v.x * view.m00 + v.y * view.m10 + v.z * view.m20;
	ray.dir.y = v.x * view.m01 + v.y * view.m11 + v.z * view.m21;
	ray.dir.z = v.x * view.m02 + v.y * view.m12 + v.z * view.m22;
	ray.org.x = view.m30;
	ray.org.y = view.m31;
	ray.org.z = view.m32;

	return ray;
}
