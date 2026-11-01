#include "ellipse.hpp"
#include <algorithm>

bool shirokov::Ellipse::isInside(double px, double py) const noexcept
{
  const double nx = (px - x) / hr;
  const double ny = (py - y) / vr;
  return nx * nx + ny * ny <= 1.0;
}

shirokov::Ellipse::Ellipse(double x, double y, double hr, double vr):
  x(x),
  y(y),
  hr(hr),
  vr(vr)
{}

shirokov::MinMaxes shirokov::Ellipse::getMinMaxes() const noexcept
{
  return {x - hr, x + hr, y - vr, y + vr};
}

shirokov::MinMaxes shirokov::getAllMinMaxes(const ellipse_list& ellipses)
{
  MinMaxes res_mm = ellipses[0].getMinMaxes();

  for (const Ellipse& s : ellipses)
  {
    const MinMaxes curr_mm = s.getMinMaxes();
    res_mm.max_x = std::max(res_mm.max_x, curr_mm.max_x);
    res_mm.min_x = std::min(res_mm.min_x, curr_mm.min_x);
    res_mm.max_y = std::max(res_mm.max_y, curr_mm.max_y);
    res_mm.min_y = std::min(res_mm.min_y, curr_mm.min_y);
  }

  return res_mm;
}
