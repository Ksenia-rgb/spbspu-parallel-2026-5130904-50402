#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include "area.hpp"
#include "input.hpp"

namespace {
  long long parseInteger(const char * str)
  {
    const std::string text(str);
    std::size_t end = 0;
    const long long value = std::stoll(text, &end);
    if (end != text.size()) {
      throw std::invalid_argument("Invalid number: " + text);
    }
    return value;
  }
}

int main(int argc, char ** argv)
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: lab threads tries [seed]\n";
    return 1;
  }
  try {
    const long long threads = parseInteger(argv[1]);
    const long long tries = parseInteger(argv[2]);
    const long long seed = (argc == 4) ? parseInteger(argv[3]) : 0;
    if (tries <= 0) {
      throw std::invalid_argument("Tries must be positive");
    }
    if (threads < 0 || seed < 0) {
      throw std::invalid_argument("Threads and seed must not be negative");
    }
    const samarin::shapes_t shapes = samarin::readShapes(std::cin);
    const std::size_t threadCount = static_cast< std::size_t >(threads);
    const std::size_t tryCount = static_cast< std::size_t >(tries);
    const std::size_t seedValue = static_cast< std::size_t >(seed);
    const samarin::areas_t areas = samarin::getAreas(shapes, threadCount, tryCount, seedValue);
    std::cout.precision(std::numeric_limits< double >::max_digits10);
    std::cout << areas.first << ' ' << areas.second << '\n';
  } catch (const std::logic_error & e) {
    std::cerr << e.what() << '\n';
    return 1;
  } catch (const std::exception & e) {
    std::cerr << "Internal error: " << e.what() << '\n';
    return 2;
  }
}
