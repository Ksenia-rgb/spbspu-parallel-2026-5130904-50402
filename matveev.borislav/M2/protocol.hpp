#ifndef MATVEEV_PROTOCOL_HPP
#define MATVEEV_PROTOCOL_HPP

#include <cstdint>
#include <vector>
#include "shape.hpp"

namespace matveev
{
  struct request_t
  {
    std::uint64_t threads = 0;
    std::uint64_t tries = 0;
    std::uint64_t count = 0;
  };
  struct response_t
  {
    double milliseconds = 0.0;
    double area = 0.0;
    std::uint64_t success = 0;
  };
  std::vector< char > makeRequest(const std::vector< shape_t > &shapes,
      std::uint64_t threads, std::uint64_t tries);
  int runWorker(int descriptor, std::uint64_t seed);
}

#endif
