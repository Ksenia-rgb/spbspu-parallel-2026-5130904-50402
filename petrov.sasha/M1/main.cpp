#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "montecarlo.hpp"

namespace petrov
{
  namespace
  {
    constexpr int minimalArgumentCount = 3;
    constexpr int maximalArgumentCount = 4;
    constexpr int threadsArgumentIndex = 1;
    constexpr int triesArgumentIndex = 2;
    constexpr int seedArgumentIndex = 3;
    constexpr std::size_t defaultSeed = 0;

    struct parameters_t
    {
      std::size_t threadCount;
      std::size_t tryCount;
      std::size_t seed;
    };

    std::size_t parseSize(const char *text)
    {
      std::size_t parsedLength = 0;
      long long value = 0;
      try
      {
        value = std::stoll(text, &parsedLength);
      } catch (const std::exception &) {
        throw std::runtime_error(std::string("incorrect value of parameter"));
      }
      if ((parsedLength != std::strlen(text)) || (value < 0))
      {
        throw std::runtime_error(std::string("incorrect value of parameter"));
      }
      return static_cast< std::size_t >(value);
    }

    parameters_t parseParameters(int argc, const char *const *argv)
    {
      if ((argc != minimalArgumentCount) && (argc != maximalArgumentCount))
      {
        throw std::runtime_error("incorrect number of arguments");
      }
      const std::size_t threadCount = parseSize(argv[threadsArgumentIndex]);
      const std::size_t tryCount = parseSize(argv[triesArgumentIndex]);
      if (tryCount == 0)
      {
        throw std::runtime_error("the number of tries must be >0");
      }
      const std::size_t seed = (argc == maximalArgumentCount) ? parseSize(argv[seedArgumentIndex]) : defaultSeed;
      return { threadCount, tryCount, seed };
    }

    void runMonteCarlo(const std::vector< circle_t > &circles, const box_t &box,
        std::size_t tries, std::size_t seed, hits_t &result)
    {
      result = countHits(circles, box, tries, seed);
    }

    hits_t countHitsInThreads(const std::vector< circle_t > &circles, const box_t &box,
        const parameters_t &parameters)
    {
      const std::size_t requestedCount = (parameters.threadCount == 0) ? 1 : parameters.threadCount;
      const std::size_t hardwareCount = static_cast< std::size_t >(std::thread::hardware_concurrency());
      const std::size_t hardwareLimit = (hardwareCount == 0) ? 1 : hardwareCount;
      const std::size_t workerCount = std::min(requestedCount, hardwareLimit);
      const std::size_t triesPerWorker = parameters.tryCount / workerCount;
      const std::size_t remainder = parameters.tryCount % workerCount;

      std::vector< hits_t > results(workerCount);
      std::vector< std::thread > workers;
      workers.reserve(workerCount);

      for (std::size_t workerIndex = 0; workerIndex < workerCount; ++workerIndex)
      {
        const std::size_t workerTries = triesPerWorker + ((workerIndex < remainder) ? 1 : 0);
        const std::size_t workerSeed = parameters.seed + workerIndex;
        workers.emplace_back(runMonteCarlo, std::cref(circles), std::cref(box), workerTries, workerSeed,
            std::ref(results[workerIndex]));
      }
      for (std::thread &worker : workers)
      {
        worker.join();
      }

      hits_t totalHits = { 0, 0 };
      for (const hits_t &hits : results)
      {
        totalHits.unionCount += hits.unionCount;
        totalHits.intersectionCount += hits.intersectionCount;
      }
      return totalHits;
    }
  }
}

int main(int argc, char **argv)
{
  try
  {
    const petrov::parameters_t parameters = petrov::parseParameters(argc, argv);
    const std::vector< petrov::circle_t > circles = petrov::readCircles(std::cin);
    const petrov::box_t box = petrov::findBoundingBox(circles);
    const petrov::hits_t hits = petrov::countHits(circles, box, parameters.tryCount, parameters.seed);
    const double coveredArea = petrov::computeArea(box, hits.unionCount, parameters.tryCount);
    const double intersectionArea = petrov::computeArea(box, hits.intersectionCount, parameters.tryCount);
    std::cout << coveredArea << ' ' << intersectionArea << '\n';
  }
  catch (const std::exception &exception)
  {
    std::cerr << exception.what() << '\n';
    return 1;
  }
  return 0;
}
