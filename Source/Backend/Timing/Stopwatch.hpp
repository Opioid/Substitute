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

    double elapsed_time() const;

private:

    uint64_t current_time() const;

    uint64_t start_time_;

    double elapsed_time_;

    double frequency_;
};

}
