#include "shapes.hpp"
#include <future>
#include <random>
#include <limits>

namespace vishnevskiy
{
  bool isInside(long long x, long long y, Circle& c)
  {
    long long rx = x - c.x;
    long long ry = y - c.y;
    return (rx * rx + ry * ry) <= (c.r * c.r);
  }

  CircleBoundaries getBoundaries(std::vector<Circle>& circles);
  {
    CircleBoundaries b
    {
      std::numeric_limits<long long>::max(),
      std::numeric_limits<long long>::lowest(),
      std::numeric_limits<long long>::max(),
      std::numeric_limits<long long>::lowest()
    };
    for (size_t i = 0; i < c.size(); ++i)
    {
      const Circle& c = circles[i];
      long long curMinX = c.x - c.radius;
      long long curMaxX = c.x + c.radius;
      long long curMinY = c.y - c.radius;
      long long curMaxY = c.y + c.radius;
      if (curMinX < b.minX)
      {
        b.minX = curMinX;
      }
      if (curMaxX > b.maxX)
      {
        b.maxX = curMaxX;
      }
      if (curMinY < b.minY)
      {
        b.minY = curMinY;
      }
      if (curMaxY > b.maxY)
      {
        b.maxY = curMaxY;
      }
    }
    return b;
  }

  void generatePoint(std::mt19937_64& gen, CircleBoundaries& b, double& px, double& py)
  {
    std::uniform_real_distribution<double> distX(static_cast<double>(b.minX), static_cast<double>(b.maxX));
    std::uniform_real_distribution<double> distY(static_cast<double>(b.minY), static_cast<double>(b.maxY));
    px = distX(gen);
    py = distY(gen);
  }
}
