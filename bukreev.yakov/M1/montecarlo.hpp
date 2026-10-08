#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include <vector>
#include <cstddef>

namespace bukreev
{
    struct Figure
    {
        int r;
        int cx, cy;
    };

    struct AreaResult
    {
        double total;
        double intersection;
    };

    AreaResult monteCarlo(
        const std::vector< Figure >& figures,
        size_t threads,
        size_t tries,
        size_t seed
    );
}

#endif
