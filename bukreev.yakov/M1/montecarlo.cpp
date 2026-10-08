#include "montecarlo.hpp"
#include <random>
#include <thread>

namespace bukreev
{
    struct BoundingBox
    {
        int left;
        int right;
        int top;
        int bottom;
    };

    void monteCarloWorker(
        const std::vector< Figure >& figures,
        const BoundingBox box,
        size_t tries, size_t seed,
        AreaResult& res
    );

    bool isInside(double x, double y, Figure f);
}

bukreev::AreaResult bukreev::monteCarlo(
    const std::vector< Figure >& figures,
    size_t threads, size_t tries, size_t seed
)
{
    BoundingBox box;
    if (!figures.empty())
    {
        Figure f = figures.front();
        box.left = f.cx - f.r;
        box.right = f.cx + f.r;
        box.top = f.cy + f.r;
        box.bottom = f.cy - f.r;
    }
    for (const Figure& f : figures)
    {
        box.left = std::min(box.left, f.cx - f.r);
        box.bottom = std::min(box.bottom, f.cy - f.r);
        box.right = std::max(box.right, f.cx + f.r);
        box.top = std::max(box.top, f.cy + f.r);
    }

    std::vector< std::thread > threadVec;
    threadVec.reserve(threads);
    std::vector< AreaResult > results(threads);

    for (size_t i = 0; i < threads; i++)
    {
        threadVec.emplace_back(
            monteCarloWorker,
            figures, box,
            tries, seed + i, std::ref(results[i])
        );
    }
    for (size_t i = 0; i < threads; i++)
    {
        threadVec[i].join();
    }

    AreaResult finalResult{0, 0};
    for (const AreaResult r : results)
    {
        finalResult.total += r.total;
        finalResult.intersection += r.intersection;
    }

    double boxArea = (box.right - box.left) * (box.top - box.bottom);
    finalResult.total = finalResult.total / threads * boxArea;
    finalResult.intersection = finalResult.intersection / threads * boxArea;
    return finalResult;
}

void bukreev::monteCarloWorker(
    const std::vector< Figure >& figures,
    BoundingBox box,
    size_t tries, size_t seed,
    AreaResult& res
)
{
    std::default_random_engine gen(seed);
    std::uniform_real_distribution< double > xdist(box.left, box.right);
    std::uniform_real_distribution< double > ydist(box.bottom, box.top);

    size_t total = 0;
    size_t intersect = 0;
    for (size_t i = 0; i < tries; i++)
    {
        double x = xdist(gen);
        double y = ydist(gen);
        bool inside = false;
        bool insideIntersect = true;
        for (const Figure& f : figures)
        {
            inside = inside || isInside(x, y, f);
            insideIntersect = insideIntersect && isInside(x, y, f);
        }

        total += inside ? 1 : 0;
        intersect += insideIntersect ? 1 : 0;
    }

    res.total = double(total) / double(tries);
    res.intersection = double(intersect) / double(tries);
}

bool bukreev::isInside(double x, double y, Figure f)
{
    double dx = x - f.cx;
    double dy = y - f.cy;
    return dx * dx + dy * dy < f.r * f.r;
}
