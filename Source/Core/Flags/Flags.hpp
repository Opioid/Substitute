#pragma once

#include <cstdint>

template<typename Enum, typename T = uint32_t>
class Flags
{

public:

	Flags(T flags = T(0)) : flags_(flags)
	{}

	inline bool is_set(Enum flag) const
	{
		T f = static_cast<T>(flag);

		return f == (flags_ & f);
	}

	inline void set(Enum flag, bool value)
	{
		T f = static_cast<T>(flag);

		value ? flags_ |= f : flags_ &= ~f;
	}

	inline explicit operator T() const
	{
		return flags_;
	}

	inline T data() const
	{
		return flags_;
	}

private:

	T flags_;
};
