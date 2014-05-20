#pragma once

#include <cstdint>

template<typename Enum, typename T>
class tFlags
{

public:

	tFlags() : flags_(0)
	{}

	bool is_set(Enum flag) const
	{
		T f = static_cast<T>(flag);

		return f == (flags_ & f);
	}

	void set(Enum flag, bool value)
	{
		T f = static_cast<T>(flag);

		value ? flags_ |= f : flags_ &= ~f;
	}

    explicit operator T() const
	{
		return flags_;
	}

private:

	T flags_;
};

typedef tFlags<uint32_t, uint32_t> Flags;


