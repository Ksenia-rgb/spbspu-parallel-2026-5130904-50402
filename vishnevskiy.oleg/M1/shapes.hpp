#ifndef SHAPES_H
#define SHAPES_H

namespace vishnevskiy
{
  struct Circle
  {
    long long r, x, y;
  };

  struct CircleBoundaries
  {
    long long minX;
    long long maxX;
    long long minY;
    long long maxY;

    long long width()
    {
      return maxX - minX;
    }
    long long height()
    {
      return maxY - minY;
    }
    long long area()
    {
      return width() * height();
    }
  };

  bool isInside(long long x, long long y, Circle& c);
  CircleBoundaries getBoundaries(std::vector<Circle>& cirkles);
}
#endif
