#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include "circle.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace novikov
{
  class MinMaxes
  {
  public:
    MinMaxes();
    double getMinX() const;
    double getMinY() const;
    double getMaxX() const;
    double getMaxY() const;
    void setMinX(double v);
    void setMinY(double v);
    void setMaxX(double v);
    void setMaxY(double v);

  private:
    double min_x_, max_x_;
    double min_y_, max_y_;
  };

  std::size_t countInside(const std::vector< Circle >& circles, double x, double y);

  MinMaxes getMinMaxes(const std::vector< Circle >& circles);

  std::pair< std::size_t, std::size_t > calc(
      const std::vector< Circle >& circles, std::size_t tries, std::size_t seed, const MinMaxes& min_maxes);

  std::pair< double, double > area(
      const std::vector< Circle >& circles, std::size_t threads, std::size_t tries, std::size_t seed);
}

#endif
