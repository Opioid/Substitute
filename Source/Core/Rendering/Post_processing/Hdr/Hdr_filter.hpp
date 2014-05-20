#pragma once

#include "Rendering/Post_processing/Post_processing_effect.hpp"
#include "Rendering/Data_format.hpp"

namespace rendering
{

class Shader_resource_view;

class Hdr_filter : public Post_processing_effect
{

public:

	Hdr_filter(Rendering_tool& rendering_tool, Data_format::Value hdr_format);
	virtual ~Hdr_filter();

	void set_exposure(float exposure);
	void set_linear_white(const float3& white);

protected:

	Data_format::Value hdr_format_;

	float  exposure_;
	float3 linear_white_;
};

}
