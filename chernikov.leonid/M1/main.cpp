#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include "multiple_area.hpp"

namespace {
  long long parseInteger(const char *text)
  {
    const std::string str(text);
    size_t pos = 0;
    const long long value = std::stoll(str, &pos);
    if (pos != str.size()) {
      throw std::invalid_argument("not an integer");
    }
    return value;
  }

  chernikov::circles_t readCircles(std::istream &in)
  {
    chernikov::circles_t circles;
    long long r = 0;
    while (in >> r) {
      long long ignored = 0;
      long long x = 0;
      long long y = 0;
      if (!(in >> ignored >> x >> y)) {
        throw std::invalid_argument("incomplete shape");
      }
      const chernikov::point_t center = {static_cast< double >(x), static_cast< double >(y)};
      circles.push_back({static_cast< double >(r), center});
    }
    if (!in.eof()) {
      throw std::invalid_argument("bad shape");
    }
    return circles;
  }
}

int main(int argc, char *argv[])
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: " << argv[0] << " threads tries [seed]\n";
    return 1;
  }
  long long threads = 0;
  long long tries = 0;
  long long seed = 0;
  try {
    threads = parseInteger(argv[1]);
    tries = parseInteger(argv[2]);
    if (argc == 4) {
      seed = parseInteger(argv[3]);
    }
  } catch (const std::exception &) {
    std::cerr << "Arguments must be integers\n";
    return 1;
  }
  if (tries <= 0) {
    std::cerr << "Number of tries must be positive\n";
    return 1;
  }
  if (threads < 0 || seed < 0) {
    std::cerr << "Threads and seed must not be negative\n";
    return 1;
  }
  chernikov::circles_t circles;
  try {
    circles = readCircles(std::cin);
  } catch (const std::invalid_argument &e) {
    std::cerr << "Cannot read figures: " << e.what() << "\n";
    return 1;
  }
  try {
    const std::pair< double, double > result = chernikov::area(circles, static_cast< size_t >(threads), static_cast< size_t >(tries), static_cast< size_t >(seed));
    std::cout << result.first << " " << result.second << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 2;
  }
  return 0;
}