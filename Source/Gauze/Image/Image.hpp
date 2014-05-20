#pragma once

#include "Image_buffer.hpp"
#include <vector>

template<typename T>
class Image
{

public:

    Image()
    {}

    Image(rendering::Data_format::Value format, const uint2& dimensions)
    {
        init(format, dimensions);
    }

    Image(rendering::Data_format::Value format, const uint2& dimensions, const T& color)
    {
        init(format, dimensions, color);
    }

    Image(const Image_buffer<T>& source)
    {
        init(source);
    }

    ~Image()
    {
        for (auto i : image_buffers_)
        {
            delete i;
        }
    }

    bool empty() const
    {
        return image_buffers_.empty();
    }

    void init(rendering::Data_format::Value format, const uint2& dimensions)
    {
        if (!empty())
        {
            return;
        }

        Image_buffer<T>* buffer = new Image_buffer<T>(format, dimensions);

        image_buffers_.push_back(buffer);
    }

    void init(rendering::Data_format::Value format, const uint2& dimensions, const T& color)
    {
        if (!empty())
        {
            return;
        }

        Image_buffer<T>* buffer = new Image_buffer<T>(format, dimensions, color);

        image_buffers_.push_back(buffer);
    }

    void init(const Image_buffer<T>& source)
    {
        if (!empty())
        {
            return;
        }

        Image_buffer<T>* buffer = new Image_buffer<T>(source);

        image_buffers_.push_back(buffer);
    }

    uint32_t get_num_levels() const
    {
        return static_cast<uint32_t>(image_buffers_.size());
    }

    const Image_buffer<T>* get_level(uint32_t index) const
    {
        if (index < get_num_levels())
        {
            return image_buffers_[index];
        }
        else
        {
            return nullptr;
        }
    }

    Image_buffer<T>* get_level(uint32_t index)
    {
        if (index < get_num_levels())
        {
            return image_buffers_[index];
        }
        else
        {
            return nullptr;
        }
    }

    void allocate_mip_chain()
    {
        rendering::Data_format::Value format = image_buffers_[0]->get_format();

        uint2 dimensions = image_buffers_[0]->get_dimensions();

        while (dimensions.x > 1 || dimensions.y > 1)
        {
            dimensions.x = std::max(dimensions.x / 2, uint32_t(1));
            dimensions.y = std::max(dimensions.y / 2, uint32_t(1));

            Image_buffer<T>* buffer = new Image_buffer<T>(format, dimensions);

            image_buffers_.push_back(buffer);
        }
    }

private:

    std::vector<Image_buffer<T>*> image_buffers_;
};
