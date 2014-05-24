#pragma once

#include <cstddef>

class Resource
{

public:

	virtual ~Resource()
	{}

	virtual size_t num_bytes() const
	{
		return 0;
	}
};
