#include "shapes.hpp"

#include <algorithm>

namespace karpenkov
{
  Box boundingBox(const std::vector< Circle > &circles)
  {
    Box box{circles[0].left(), circles[0].right(), circles[0].bottom(), circles[0].top()};
    for (const Circle &c : circles)
    {
      box.min_x = std::min(box.min_x, c.left());
      box.max_x = std::max(box.max_x, c.right());
      box.min_y = std::min(box.min_y, c.bottom());
      box.max_y = std::max(box.max_y, c.top());
    }
    return box;
  }
}