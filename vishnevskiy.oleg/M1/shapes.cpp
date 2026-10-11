#include "shapes.hpp"
#include <future>
#include <random>

namespace vishnevskiy
{
  bool isInside(int x, int y, Circle& c)
  {
    int rx = x - c.x;
    int ry = y - c.y;
    return (rx * rx + ry * ry) <= (c.r * c.r);
  }
}
