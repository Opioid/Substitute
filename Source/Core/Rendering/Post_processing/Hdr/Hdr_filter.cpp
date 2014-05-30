#include "Hdr_filter.hpp"
#include "Math/Vector.inl"

namespace rendering
{

Hdr_filter::Hdr_filter(Rendering_tool& rendering_tool, Data_format::Value hdr_format) :
	Post_processing_effect(rendering_tool),
	hdr_format_(hdr_format),
	exposure_(1.f),
	linear_white_(11.2f, 11.2f, 11.2f)
{}

Hdr_filter::~Hdr_filter()
{}

void Hdr_filter::set_exposure(float exposure)
{
	exposure_ = exposure;
}

void Hdr_filter::set_linear_white(const float3& white)
{
	linear_white_ = white;
}

}
