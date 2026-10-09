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
  }
}

int main(int argc, char **argv)
{
  try
  {
    const petrov::parameters_t parameters = petrov::parseParameters(argc, argv);
  }
  catch (const std::exception &exception)
  {
    std::cerr << exception.what() << '\n';
    return 1;
  }
  return 0;
}
