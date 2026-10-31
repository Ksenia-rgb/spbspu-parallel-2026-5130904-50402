#ifndef CALCULATION_HPP
#define CALCULATION_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "shapes.hpp"

namespace pozdnyakov
{
  enum class calculation_status_t
  {
    in_progress,
    done,
    canceled
  };

  struct calculation_request_t
  {
    std::size_t threads;
    std::size_t tries;
    ellipses_t ellipses;
  };

  struct calculation_result_t
  {
    double milliseconds;
    double area;
  };

  struct calculation_t
  {
    std::string process;
    calculation_status_t status;
    calculation_result_t result;
  };


}

#endif
