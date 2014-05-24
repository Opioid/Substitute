#pragma once

#include <cstddef>

template<typename T> 
class Handle
{

public:

	Handle() : resource_(nullptr)
	{}

	Handle(T* resource) : resource_(resource)
	{}

	Handle(const Handle& handle) : resource_(handle.resource_)
	{
		increment_reference_count();
	}

	~Handle()
	{
		release();
	}

	Handle& operator=(const Handle& handle)
	{
		if (resource_ != handle.resource_)
		{
			release();

			resource_ = handle.resource_;

			increment_reference_count();
		}

		return *this;
	}

	const T* operator->() const
	{
		return resource_;
	}

	T* operator->()
	{
		return resource_;
	}

	operator const T*() const
	{
		return resource_;
	}

	operator T*()
	{
		return resource_;
	}

	size_t reference_count() const
	{
		return resource_->reference_count_;
	}

private:

	void increment_reference_count()
	{
		if (resource_)
		{
			++resource_->reference_count_;
		}
	}

	void release()
	{
		if (resource_ && --resource_->reference_count_ == 0)
		{
			delete resource_;
			resource_ = nullptr;
		}
	}

	T* resource_;
};
