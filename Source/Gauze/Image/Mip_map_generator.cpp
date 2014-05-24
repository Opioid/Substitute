#include "Mip_map_generator.hpp"
#include "Sampler/Point_sampler.hpp"

Image<rendering::Color4>* Mip_map_generator::create_mip_mapped_image(const Image_buffer<rendering::Color4>& source)
{
	Image<rendering::Color4>* image = new Image<rendering::Color4>(source);

	image->allocate_mip_chain();

	Point_sampler<rendering::Color4> sampler(source);

	// Start at mip level 1, as level 0 is the unmodified source buffer that we just initialized the image with

	for (uint32_t i = 1; i < image->num_levels(); ++i)
	{
		Image_buffer<rendering::Color4>* mip = image->get_level(i);

		const uint2& dimensions = mip->dimensions();

		float2 texel_size = float2(1.f / float(dimensions.x), 1.f / float(dimensions.y));

		float2 coordinates = 0.5f * texel_size;

		for (uint32_t y = 0; y < dimensions.y; ++y)
		{
			coordinates.x = 0.f;

			for (uint32_t x = 0; x < dimensions.x; ++x)
			{
				rendering::Color4 source_color = sampler.sample(coordinates);

				mip->set(uint2(x, y), source_color);

				coordinates.x += texel_size.x;
			}

			coordinates.y += texel_size.y;
		}
	}

    return image;
}
