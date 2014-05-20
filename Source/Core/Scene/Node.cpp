#include "Node.hpp"

namespace scene
{

Node::Node() : local_transformation_(Transformation::identity), world_transformation_(float4x4::identity), parent_(nullptr), next_(nullptr), child_(nullptr)
{}

const float3& Node::get_local_position() const
{
	return local_transformation_.position;
}

void Node::set_local_position(const float3& position)
{
	local_transformation_.position = position;
}

const float3& Node::get_local_scale() const
{
	return local_transformation_.scale;
}

void Node::set_local_scale(const float3& scale)
{
	local_transformation_.scale = scale;
}

const Quaternion& Node::get_local_rotation() const
{
	return local_transformation_.rotation;
}

void Node::set_local_rotation(const Quaternion& rotation)
{
	local_transformation_.rotation = rotation;
}

Transformation const& Node::get_local_transformation() const
{
	return local_transformation_;
}

float3 Node::get_world_position() const
{
	return world_transformation_.rows[3].xyz;
}

float3 Node::get_world_scale() const
{
	return ::get_scale(world_transformation_);
}

float3 Node::get_world_direction() const
{
	return normalize(world_transformation_.rows[2].xyz); 
}

float3 Node::get_world_up() const
{
	return normalize(world_transformation_.rows[1].xyz); 
}

float3 Node::get_world_right() const
{
	return normalize(world_transformation_.rows[0].xyz); 
}

float3x3 Node::get_world_rotation() const
{
	return get_unscaled_basis(world_transformation_);
}

const float4x4& Node::get_world_transformation() const
{
	return world_transformation_;
}

void Node::set_world_transformation(const float3& position, const float3& scale, const float3x3& rotation)
{
	set_basis(world_transformation_, rotation);
	::scale(world_transformation_, scale);
	set_origin(world_transformation_, position);
}

void Node::fix_world_transformation()
{
	set_world_transformation(local_transformation_.position, local_transformation_.scale, float3x3(local_transformation_.rotation));
}

void Node::propagate_transformation()
{
	if (child_)
	{
		child_->inherit_transformation(get_world_rotation(), world_transformation_);
	}
}

void Node::inherit_transformation(const float3x3& parent_rot, const float4x4& parent_transformation)
{
	if (next_)
	{
		next_->inherit_transformation(parent_rot, parent_transformation);
	}

	// Using the get_world*() methods is important, because they (might) contain interpolated data

	float3x3 world_rot = get_world_rotation() * parent_rot;
	float3 world_pos = get_world_position() * parent_transformation;

	set_world_transformation(world_pos, get_world_scale(), world_rot);

	propagate_transformation();
}

void Node::attach(Node* node)
{
	node->detach();

	node->parent_ = this;

	if (!child_)
	{
		child_ = node;
	}
	else
	{
		child_->add_sibling(node);
	}
}

void Node::detach()
{
	if (parent_)
	{
		parent_->detach(this);
	}
}

const Node* Node::get_parent() const
{
	return parent_;
}


void Node::add_sibling(Node* node)
{
	if (!next_)
	{
		next_ = node;
	}
	else
	{
		next_->add_sibling(node);
	}
}

void Node::detach(Node* node)
{
	//we can assume this to be true because of detach()
//	if (node->parent_ == this)
//	{
		node->parent_ = nullptr;

		if (child_ == node)
		{
			child_ = node->next_;
			node->next_ = nullptr;
		}
		else
		{
			child_->remove_sibling(node);
		}

//	}
}

void Node::remove_sibling(Node* node)
{
	if (next_ == node)
	{
		next_ = node->next_;
		node->next_ = nullptr;
	}
	else
	{
		next_->remove_sibling(node);
	}
}

}
