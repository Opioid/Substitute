#pragma once

#include <cstddef>

class Resource
{

public:

	virtual ~Resource()
	{}

	virtual size_t get_num_bytes() const
	{
		return 0;
	}
};
