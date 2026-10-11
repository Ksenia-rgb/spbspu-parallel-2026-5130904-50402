#ifndef SHAPES_H
#define SHAPES_H

namespace vishnevskiy
{
  struct Circle
  {
    int r, x, y;
  };

  bool isInside(int x, int y, Circle& c);
}
#endif
