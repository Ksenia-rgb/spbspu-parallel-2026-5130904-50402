#include "shape.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <ostream>
#include <iomanip>
#include <limits>

bool matveev::contains(const shape_t &shape, const double x, const double y)
{
  const double dx = x - shape.x;
  const double dy = y - shape.y;
  const double cosine = std::cos(shape.angle);
  const double sine = std::sin(shape.angle);
  const double horizontal = (dx * cosine + dy * sine) / shape.radiusX;
  const double vertical = (-dx * sine + dy * cosine) / shape.radiusY;
  constexpr double unitRadiusSquared = 1.0;
  return horizontal * horizontal + vertical * vertical <= unitRadiusSquared;
}

matveev::frame_t matveev::getFrame(const shape_t &shape)
{
  const long double cosine = std::cos(static_cast< long double >(shape.angle));
  const long double sine = std::sin(static_cast< long double >(shape.angle));
  const long double width = std::hypot(shape.radiusX * cosine, shape.radiusY * sine);
  const long double height = std::hypot(shape.radiusX * sine, shape.radiusY * cosine);
  return {shape.x - width, shape.y - height, shape.x + width, shape.y + height};
}

matveev::frame_t matveev::getFrame(const std::vector< shape_t > &shapes)
{
  if (shapes.empty())
  {
    throw std::invalid_argument("Empty set");
  }
  frame_t result = getFrame(shapes.front());
  for (const shape_t &shape: shapes)
  {
    const frame_t current = getFrame(shape);
    result.left = std::min(result.left, current.left);
    result.bottom = std::min(result.bottom, current.bottom);
    result.right = std::max(result.right, current.right);
    result.top = std::max(result.top, current.top);
  }
  return result;
}

std::vector< matveev::shape_t > matveev::translateShapes(const std::vector< shape_t > &shapes)
{
  if (shapes.empty())
  {
    throw std::invalid_argument("Empty set");
  }
  long long originX = shapes.front().x;
  long long originY = shapes.front().y;
  constexpr long long minimum = std::numeric_limits< long long >::min();
  constexpr long long maximum = std::numeric_limits< long long >::max();
  for (const shape_t &shape: shapes)
  {
    if (((originX > 0) && (shape.x < minimum + originX))
        || ((originX < 0) && (shape.x > maximum + originX)))
    {
      originX = 0;
    }
    if (((originY > 0) && (shape.y < minimum + originY))
        || ((originY < 0) && (shape.y > maximum + originY)))
    {
      originY = 0;
    }
  }
  std::vector< shape_t > result = shapes;
  for (shape_t &shape: result)
  {
    shape.x -= originX;
    shape.y -= originY;
  }
  return result;
}

void matveev::printShape(std::ostream &out, const shape_t &shape)
{
  out << shape.radiusX;
  if (shape.ellipse)
  {
    out << " " << shape.radiusY;
  }
  out << " (" << shape.x << " " << shape.y << ")";
  if (shape.ellipse)
  {
    const std::ios_base::fmtflags flags = out.flags();
    const std::streamsize precision = out.precision();
    constexpr int anglePrecision = 3;
    out << " " << std::fixed << std::setprecision(anglePrecision) << shape.angle;
    out.flags(flags);
    out.precision(precision);
  }
  out << "\n";
}

void matveev::printFrame(std::ostream &out, const frame_t &frame)
{
  const std::streamsize precision = out.precision();
  out << std::setprecision(std::numeric_limits< long double >::max_digits10);
  out << "(" << frame.left << " " << frame.bottom << ") (";
  out << frame.right << " " << frame.top << ")\n";
  out.precision(precision);
}
