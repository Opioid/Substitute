#pragma once

#include <cstdint>

namespace timing
{

class Stopwatch
{

public:

    Stopwatch();

	void start();

    double update();

    double stop();

    double get_elapsed_time() const;

private:

    uint64_t get_current_time() const;

    uint64_t m_start_time;

    double m_elapsed_time;

    double m_frequency;
};

}
