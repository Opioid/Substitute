#pragma once

#include "Constant_buffer_adapter.hpp"
#include "Effect.hpp"

namespace rendering
{

class Effect;

template<typename T>
class Constant_buffer_updater
{

public:

    Constant_buffer_updater() : adapter_(nullptr)
    {}

	bool init(Constant_buffer_adapter* adapter)
	{
		adapter_ = adapter;

		if (!adapter_)
		{
			return false;
		}

		return true;
	}

	bool init(const Effect* effect, const std::string& buffer_name)
	{
		adapter_ = effect->constant_buffer_adapter(buffer_name);

		if (!adapter_)
		{
			return false;
		}

		return true;
	}

	T& data()
	{
		return data_;
	}

	Constant_buffer_adapter* adapter()
	{
		return adapter_;
	}

	void update(const Rendering_device& device)
	{
		adapter_->update(sizeof(T), &data_, device);
	}

private:

	T data_;

    Constant_buffer_adapter* adapter_;
};

}


