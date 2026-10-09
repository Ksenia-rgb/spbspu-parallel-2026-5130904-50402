#include "montecarlo.hpp"

#include <algorithm>
#include <istream>
#include <stdexcept>

std::istream &petrov::operator>>(std::istream &in, p_t &point)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  return in >> point.x >> point.y;
}

std::istream &petrov::operator>>(std::istream &in, circle_t &circle)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  double ignoredParameter = 0.0;
  return in >> circle.radius >> ignoredParameter >> circle.center;
}

std::vector< petrov::circle_t > petrov::readCircles(std::istream &in)
{
  std::vector< circle_t > circles;
  while (true)
  {
    in >> std::ws;
    if (in.eof())
    {
      break;
    }
    circle_t circle = {};
    if (!(in >> circle))
    {
      throw std::runtime_error("incorrect input data");
    }
    circles.push_back(circle);
  }
  return circles;
}

petrov::box_t petrov::findBoundingBox(const std::vector< circle_t > &circles)
{
  box_t box = { { 0.0, 0.0 }, { 0.0, 0.0 } };
  if (circles.empty())
  {
    return box;
  }
  const circle_t &firstCircle = circles.front();
  box.minPoint.x = firstCircle.center.x - firstCircle.radius;
  box.minPoint.y = firstCircle.center.y - firstCircle.radius;
  box.maxPoint.x = firstCircle.center.x + firstCircle.radius;
  box.maxPoint.y = firstCircle.center.y + firstCircle.radius;

  for (const circle_t &circle : circles)
  {
    box.minPoint.x = std::min(box.minPoint.x, circle.center.x - circle.radius);
    box.minPoint.y = std::min(box.minPoint.y, circle.center.y - circle.radius);
    box.maxPoint.x = std::max(box.maxPoint.x, circle.center.x + circle.radius);
    box.maxPoint.y = std::max(box.maxPoint.y, circle.center.y + circle.radius);
  }
  return box;
}

double petrov::computeBoxArea(const box_t &box)
{
  const double width = box.maxPoint.x - box.minPoint.x;
  const double height = box.maxPoint.y - box.minPoint.y;
  return width * height;
}
