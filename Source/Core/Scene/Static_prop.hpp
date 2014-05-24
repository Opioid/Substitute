#pragma once

#include "Prop.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

namespace scene
{

class Static_prop : public Prop
{

public:

	Static_prop();

	float3 world_position() const;

	float3 world_direction() const;
	float3 world_right() const;
	float3 world_up() const;

	float3x3 world_rotation() const;

	const float4x4& world_transformation() const;

	void set_world_transformation(const float3& position, const float3& scale, const float3x3& rotation);

    bool create_surfaces(const Handle<Model>& model, uint32_t num_materials, const Handle<Material> materials[]);

private:

	float4x4 world_transformation_;
};

}
