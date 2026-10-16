#include "figure.hpp"

#include <istream>
#include <stdexcept>
#include <vector>

sogdanov::Figure::Figure(const long long horizontal_radius, const long long vertical_radius, const long long center_x,
    const long long center_y):
  horizontal_radius_(static_cast< long double >(horizontal_radius)),
  vertical_radius_(static_cast< long double >(vertical_radius)),
  center_x_(static_cast< long double >(center_x)),
  center_y_(static_cast< long double >(center_y))
{
  if ((horizontal_radius <= 0) || (vertical_radius <= 0))
  {
    throw std::invalid_argument("Figure radius must be positive");
  }
}

bool sogdanov::Figure::contains(const long double x, const long double y) const
{
  const long double k = 1.0;
  const long double x1 = (x - center_x_) / horizontal_radius_;
  const long double y1 = (y - center_y_) / vertical_radius_;

  return (x1 * x1 + y1 * y1) <= k;
}

sogdanov::Bounds sogdanov::Figure::getBounds() const
{
  return {center_x_ - horizontal_radius_, center_x_ + horizontal_radius_, center_y_ - vertical_radius_,
      center_y_ + vertical_radius_};
}

std::vector< sogdanov::Figure > sogdanov::readFigures(std::istream& input)
{
  std::vector< Figure > figures{};

  while (true)
  {
    input >> std::ws;
    if (input.bad())
    {
      throw std::runtime_error("Failed to read standard input");
    }
    if (input.eof())
    {
      break;
    }

    long long first = 0;
    long long second = 0;
    long long center_x = 0;
    long long center_y = 0;

    if (!(input >> first >> second >> center_x >> center_y))
    {
      throw std::invalid_argument("Expected four int for each figure");
    }

    const long long vertical_radius = (second == 0) ? first : second;

    figures.emplace_back(first, vertical_radius, center_x, center_y);
  }

  return figures;
}
