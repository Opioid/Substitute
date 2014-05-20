#pragma once

#include <cstddef>

class Reference_counter
{

public:

	Reference_counter();

private:

	size_t m_reference_count;

	template<typename> friend class Handle;
};
