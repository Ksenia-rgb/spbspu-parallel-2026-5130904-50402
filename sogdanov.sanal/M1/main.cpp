#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "figure.hpp"
#include "monte_carlo.hpp"

namespace sogdanov
{
  std::size_t parseArg(const char* t)
  {
    const std::string arg = t;

    if (arg.empty())
    {
      throw std::invalid_argument("Expected a non-empty integer arg");
    }

    if (arg == "+")
    {
      throw std::invalid_argument("Expected digits after '+'");
    }

    if (!((arg.front() >= '0') && (arg.front() <= '9')) && (arg.front() != '+'))
    {
      throw std::invalid_argument("Expected a non-negative integer arg");
    }

    std::size_t pos = 0;
    const unsigned long long value = std::stoull(arg, &pos);

    if (pos != arg.size())
    {
      throw std::invalid_argument("Expected an integer arg");
    }
    if (value > std::numeric_limits< std::size_t >::max())
    {
      throw std::out_of_range("Integer arg is out of range");
    }

    return static_cast< std::size_t >(value);
  }

}

int main(int argc, char* argv[])
{
  try
  {
    if ((argc != 3) && (argc != 4))
    {
      throw std::invalid_argument("Expected 3 or 4 arguments");
    }

    const std::size_t thread_count = sogdanov::parseArg(argv[1]);
    const std::size_t tries = sogdanov::parseArg(argv[2]);
    const std::size_t seed = (argc == 4) ? sogdanov::parseArg(argv[3]) : 0;

    if (tries == 0)
    {
      throw std::invalid_argument("The number of tries must be positive");
    }

    const std::vector< sogdanov::Figure > figures = sogdanov::readFigures(std::cin);
    const sogdanov::Areas areas = sogdanov::estimateAreas(figures, thread_count, tries, seed);

    std::cout << std::setprecision(std::numeric_limits< long double >::max_digits10) << areas.coverage << ' '
              << areas.intersection << '\n';
    if (!std::cout)
    {
      throw std::runtime_error("Failed to write standard output");
    }
  }
  catch (const std::invalid_argument& error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }
  catch (const std::out_of_range&)
  {
    std::cerr << "Integer arg is out of range\n";
    return 1;
  }
  catch (const std::exception& error)
  {
    std::cerr << error.what() << '\n';
    return 2;
  }
}
