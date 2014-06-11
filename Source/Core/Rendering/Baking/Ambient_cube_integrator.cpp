#include "Ambient_cube_integrator.hpp"
#include "Ambient_cube.hpp"
#include "Math/Vector.inl"
#include "Math/Half_vector.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "Rendering/Storage/Texture_storage_save.hpp"

namespace rendering
{

namespace baking
{

Ambient_cube_integrator::Ambient_cube_integrator(const uint2& face_dimensions) : face_dimensions_(face_dimensions), summed_weights_(0.f)
{
	weights_dimensions_ = uint2(face_dimensions.x * 3, face_dimensions.y);

	const uint2 half_face_dimensions = face_dimensions / 2;

	weights_offsets_[0] = uint2(0, 0);
	weights_offsets_[1] = uint2(face_dimensions.x, 0);
	weights_offsets_[2] = uint2(face_dimensions.x + half_face_dimensions.x, 0);
	weights_offsets_[3] = uint2(2 * face_dimensions.x, 0);
	weights_offsets_[4] = uint2(2 * face_dimensions.x, half_face_dimensions.y);

	weights_ = new float[weights_dimensions_.x * weights_dimensions_.y];

	const float2 double_pixel_size = float2(2.f / float(face_dimensions_.x), 2.f / float(face_dimensions_.y));

	// "Front"
	float3 dir(0.f, 0.f, 1.f);

	for (uint32_t y = 0; y < face_dimensions_.y; ++y)
	{
		uint32_t oy = y + weights_offsets_[0].y;

		for (uint32_t x = 0; x < face_dimensions_.x; ++x)
		{
			float3 v(-1.f + (0.5f + float(x)) * double_pixel_size.x, 1.f - (0.5f + float(y)) * double_pixel_size.y, 1.f);

			float v_dot_v = dot(v, v);
			float solid_angle = 1.f / (sqrt(v_dot_v) * v_dot_v);
			float weight = solid_angle * dot(dir, normalize(v));

			uint32_t ox = x + weights_offsets_[0].x;

			weights_[oy * weights_dimensions_.x + ox] = weight;

			summed_weights_ += weight;
		}
	}

	float local_sum = 0.f;

	// [x/2..x], [0..y]
	dir = float3(0.f, 0.f, 1.f);

	for (uint32_t y = 0; y < face_dimensions_.y; ++y)
	{
		uint32_t oy = y + weights_offsets_[1].y;

		for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
		{
			float3 v(-1.f, 1.f - (0.5f + float(y)) * double_pixel_size.y, 0.f + (0.5f + float(x)) * double_pixel_size.x);

			float v_dot_v = dot(v, v);
			float solid_angle = 1.f / (sqrt(v_dot_v) * v_dot_v);
			float weight = solid_angle * dot(dir, normalize(v));

			uint32_t ox = x + weights_offsets_[1].x;

			weights_[oy * weights_dimensions_.x + ox] = weight;

			summed_weights_ += weight;

			local_sum += weight;
		}
	}

	local_sum = 0.f;

	// [0..x/2], [0..y]
	dir = float3(0.f, 0.f, 1.f);

	for (uint32_t y = 0; y < face_dimensions_.y; ++y)
	{
		uint32_t oy = y + weights_offsets_[2].y;

		for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
		{
			float3 v(1.f, 1.f - (0.5f + float(y)) * double_pixel_size.y, 1.f - (0.5f + float(x)) * double_pixel_size.x);

			float v_dot_v = dot(v, v);
			float solid_angle = 1.f / (sqrt(v_dot_v) * v_dot_v);
			float weight = solid_angle * dot(dir, normalize(v));

			uint32_t ox = x + weights_offsets_[2].x;

			weights_[oy * weights_dimensions_.x + ox] = weight;

			summed_weights_ += weight;

			local_sum += weight;
		}
	}

	local_sum = 0.f;

	// [0..x], [y/2..y]
	dir = float3(0.f, 0.f, 1.f);

	for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
	{
		uint32_t oy = y + weights_offsets_[3].y;

		for (uint32_t x = 0; x < face_dimensions_.x; ++x)
		{
			float3 v(-1.f + (0.5f + float(x)) * double_pixel_size.x, 1.f, 0.f + (0.5f + float(y)) * double_pixel_size.y);

			float v_dot_v = dot(v, v);
			float solid_angle = 1.f / (sqrt(v_dot_v) * v_dot_v);
			float weight = solid_angle * dot(dir, normalize(v));

			uint32_t ox = x + weights_offsets_[3].x;

			weights_[oy * weights_dimensions_.x + ox] = weight;

			summed_weights_ += weight;

			local_sum += weight;
		}
	}

	local_sum = 0.f;

	// [0..x], [0..y/2]
	dir = float3(0.f, 0.f, 1.f);

	for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
	{
		uint32_t oy = y + weights_offsets_[4].y;

		for (uint32_t x = 0; x < face_dimensions_.x; ++x)
		{
			float3 v(-1.f + (0.5f + float(x)) * double_pixel_size.x, 1.f, 1.f - (0.5f + float(y)) * double_pixel_size.y);

			float v_dot_v = dot(v, v);
			float solid_angle = 1.f / (sqrt(v_dot_v) * v_dot_v);
			float weight = solid_angle * dot(dir, normalize(v));

			uint32_t ox = x + weights_offsets_[4].x;

			weights_[oy * weights_dimensions_.x + ox] = weight;

			summed_weights_ += weight;

			local_sum += weight;
		}
	}
}

Ambient_cube_integrator::~Ambient_cube_integrator()
{
	delete [] weights_;
}

void Ambient_cube_integrator::set_pitch(uint32_t pitch)
{
	pitch_ = pitch;
}

void Ambient_cube_integrator::integrate(Ambient_cube& ambient_cube, uint32_t batch_index, const unsigned char* data) const
{
	const half4* buffer = reinterpret_cast<const half4*>(data);

	for (uint32_t f = 0; f < 6; ++f)
	{
		ambient_cube.colors[f] = integrate(f, batch_index, buffer);
	}
}

Color3 Ambient_cube_integrator::integrate(uint32_t hemisphere, uint32_t batch_index, const half4* data) const
{
	static const uint32_t x_positive = 0;
	static const uint32_t x_negative = 1;
	static const uint32_t y_positive = 2;
	static const uint32_t y_negative = 3;
	static const uint32_t z_positive = 4;
	static const uint32_t z_negative = 5;

	static const uint32_t X2_X  = 1;
	static const uint32_t _0_X2 = 2;
	static const uint32_t Y2_Y  = 3;
	static const uint32_t _0_Y2 = 4;

	const uint2 half_face_dimensions = face_dimensions_ / 2;
	
	const uint32_t base_offset = pitch_ * face_dimensions_.y * batch_index;

	uint32_t face_offset = base_offset + hemisphere * face_dimensions_.x;

	Color3 accumulator(0.f, 0.f, 0.f);

	for (uint32_t y = 0; y < face_dimensions_.y; ++y)
	{
		uint32_t line_offset = face_offset + y * pitch_;

		for (uint32_t x = 0; x < face_dimensions_.x; ++x)
		{
			uint32_t index = line_offset + x;

			const half4& sample = data[index];

			float weight = weights_[y * weights_dimensions_.x + x];

			accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
		}
	}

//	if (x_positive == hemisphere && 0 == batch_index)
//	{
//		save(uint2(face_dimensions_.x * 6, face_dimensions_.y * 8), 0, data);
//	}

	uint32_t face_offset_w = 0;

	if (x_positive == hemisphere)
	{
		// +Y
		face_offset = base_offset + y_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[X2_X].y * weights_dimensions_.x + weights_offsets_[X2_X].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x + half_face_dimensions.x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Y
		face_offset = base_offset + y_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[X2_X].y * weights_dimensions_.x + weights_offsets_[X2_X].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x + half_face_dimensions.x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// +Z
		face_offset = base_offset + z_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[X2_X].y * weights_dimensions_.x + weights_offsets_[X2_X].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x + half_face_dimensions.x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Z
		face_offset = base_offset + z_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_X2].y * weights_dimensions_.x + weights_offsets_[_0_X2].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}
	}
	else if (x_negative == hemisphere)
	{
		// +Y
		face_offset = base_offset + y_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_X2].y * weights_dimensions_.x + weights_offsets_[_0_X2].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Y
		face_offset = base_offset + y_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_X2].y * weights_dimensions_.x + weights_offsets_[_0_X2].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// +Z
		face_offset = base_offset + z_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_X2].y * weights_dimensions_.x + weights_offsets_[_0_X2].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Z
		face_offset = base_offset + z_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[X2_X].y * weights_dimensions_.x + weights_offsets_[X2_X].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x + half_face_dimensions.x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}
	}
	else if (y_positive == hemisphere)
	{
		// +X
		face_offset = base_offset + x_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_Y2].y * weights_dimensions_.x + weights_offsets_[_0_Y2].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -X
		face_offset = base_offset + x_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_Y2].y * weights_dimensions_.x + weights_offsets_[_0_Y2].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// +Z
		face_offset = base_offset + z_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_Y2].y * weights_dimensions_.x + weights_offsets_[_0_Y2].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Z
		face_offset = base_offset + z_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_Y2].y * weights_dimensions_.x + weights_offsets_[_0_Y2].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}
	}
	else if (y_negative == hemisphere)
	{
		// +X
		face_offset = base_offset + x_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[Y2_Y].y * weights_dimensions_.x + weights_offsets_[Y2_Y].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + (y + half_face_dimensions.y) * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -X
		face_offset = base_offset + x_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[Y2_Y].y * weights_dimensions_.x + weights_offsets_[Y2_Y].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + (y + half_face_dimensions.y) * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// +Z
		face_offset = base_offset + z_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[Y2_Y].y * weights_dimensions_.x + weights_offsets_[Y2_Y].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + (y + half_face_dimensions.y) * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Z
		face_offset = base_offset + z_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[Y2_Y].y * weights_dimensions_.x + weights_offsets_[Y2_Y].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + (y + half_face_dimensions.y) * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}
	}
	else if (z_positive == hemisphere)
	{
		// +X
		face_offset = base_offset + x_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_X2].y * weights_dimensions_.x + weights_offsets_[_0_X2].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -X
		face_offset = base_offset + x_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[X2_X].y * weights_dimensions_.x + weights_offsets_[X2_X].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x + half_face_dimensions.x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// +Y
		face_offset = base_offset + y_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[Y2_Y].y * weights_dimensions_.x + weights_offsets_[Y2_Y].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + (y + half_face_dimensions.y) * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Y
		face_offset = base_offset + y_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_Y2].y * weights_dimensions_.x + weights_offsets_[_0_Y2].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}
	}
	else if (z_negative == hemisphere)
	{
		// +X
		face_offset = base_offset + x_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[X2_X].y * weights_dimensions_.x + weights_offsets_[X2_X].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x + half_face_dimensions.x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -X
		face_offset = base_offset + x_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_X2].y * weights_dimensions_.x + weights_offsets_[_0_X2].x;

		for (uint32_t y = 0; y < face_dimensions_.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < half_face_dimensions.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// +Y
		face_offset = base_offset + y_positive * face_dimensions_.x;
		face_offset_w = weights_offsets_[_0_Y2].y * weights_dimensions_.x + weights_offsets_[_0_Y2].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + y * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}

		// -Y
		face_offset = base_offset + y_negative * face_dimensions_.x;
		face_offset_w = weights_offsets_[Y2_Y].y * weights_dimensions_.x + weights_offsets_[Y2_Y].x;

		for (uint32_t y = 0; y < half_face_dimensions.y; ++y)
		{
			uint32_t line_offset   = face_offset + (y + half_face_dimensions.y) * pitch_;
			uint32_t line_offset_w = face_offset_w + y * weights_dimensions_.x;

			for (uint32_t x = 0; x < face_dimensions_.x; ++x)
			{
				uint32_t index = line_offset + x;

				const half4& sample = data[index];

				float weight = weights_[line_offset_w + x];

				accumulator += weight * Color3(float(sample.x), float(sample.y), float(sample.z));
			}
		}
	}

	return accumulator / summed_weights_;
}

void Ambient_cube_integrator::save(const uint2& dimensions, uint32_t offset, const half4* source_data) const
{
	Texture_description texture_description;
	texture_description.type = rendering::Texture_description::Type::Texture_2D;
	texture_description.format = rendering::Data_format::R8G8B8A8_UNorm_sRGB;
	texture_description.dimensions.xy = dimensions;

	rendering::Texture_description::Data data;
	data.dimensions = texture_description.dimensions;
	data.num_bytes = texture_description.dimensions.x * texture_description.dimensions.y * Data_format::num_bytes_per_block(texture_description.format);
	data.row_pitch = texture_description.dimensions.x * Data_format::num_bytes_per_block(texture_description.format);
	data.buffer = new unsigned char[data.num_bytes];

	Color4c* buffer = reinterpret_cast<Color4c*>(data.buffer);

	for (uint32_t y = 0; y < dimensions.y; ++y)
	{
		uint32_t line_offset = offset + y * pitch_;

		for (uint32_t x = 0; x < dimensions.x; ++x)
		{
			uint32_t index = line_offset + x;

			const half4& sample = source_data[index];

			Color3 clamped(std::min(float(sample.x), 1.f), std::min(float(sample.y), 1.f), std::min(float(sample.z), 1.f));

			Color3 sRGB = linear_to_sRGB(clamped);

			buffer[y * dimensions.x + x] = rendering::Color4c(sRGB.x * 255.f, sRGB.y * 255.f, sRGB.z * 255.f, 255);
		}
	}

	rendering::Generic_texture_data_adapter adapter(texture_description, &data);

	rendering::texture_storage::save("test.webp", adapter, file::File_type::WEBP);

	delete [] data.buffer;
}

}

}
