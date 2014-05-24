#pragma once

#include <cstddef>

class Reference_counter
{

public:

	Reference_counter();

private:

	size_t reference_count_;

	template<typename> friend class Handle;
};
