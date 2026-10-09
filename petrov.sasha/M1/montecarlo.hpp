#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <iosfwd>
#include <vector>

namespace petrov
{
  struct p_t
  {
    double x;
    double y;
  };

  struct circle_t
  {
    double radius;
    p_t center;
  };

  struct box_t
  {
    p_t minPoint;
    p_t maxPoint;
  };

  std::istream &operator>>(std::istream &in, p_t &point);
  std::istream &operator>>(std::istream &in, circle_t &circle);

  std::vector< circle_t > readCircles(std::istream &in);

  box_t findBoundingBox(const std::vector< circle_t > &circles);
  double computeBoxArea(const box_t &box);
}

#endif
