#pragma once

#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"
#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"
#include <cstdint>

namespace scene
{

class Node
{

public:

	Node();

	const float3& get_local_position() const;
	void set_local_position(const float3& position);

	const float3& get_local_scale() const;
	void set_local_scale(const float3& scale);

	const Quaternion& get_local_rotation() const;
	void set_local_rotation(const Quaternion& rotation);

	const Transformation& get_local_transformation() const;

	float3 get_world_position() const;
	float3 get_world_scale() const;

	float3 get_world_direction() const;
	float3 get_world_right() const;
	float3 get_world_up() const;

	float3x3 get_world_rotation() const;

	const float4x4& get_world_transformation() const;
	void set_world_transformation(const float3& position, const float3& scale, const float3x3& rotation);
	void fix_world_transformation();

	void propagate_transformation();
	void inherit_transformation(const float3x3& parent_rot, const float4x4& parent_transformation);

	void attach(Node* node);
	void detach();

	const Node* get_parent() const;

protected:

	Transformation local_transformation_;

private:

	float4x4 world_transformation_;

	void add_sibling(Node* node);

	void detach(Node* node);
	void remove_sibling(Node* node);

	Node* parent_;
	Node* next_;
	Node* child_;
};

}