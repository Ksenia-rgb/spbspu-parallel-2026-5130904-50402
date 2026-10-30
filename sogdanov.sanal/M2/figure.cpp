#include "figure.hpp"

#include <istream>
#include <stdexcept>
#include <vector>

sogdanov::Figure::Figure(const long long horizontal_radius, const long long vertical_radius, const long long center_x,
    const long long center_y):
  horizontal_radius_(horizontal_radius),
  vertical_radius_(vertical_radius),
  center_x_(center_x),
  center_y_(center_y)
{
  if ((horizontal_radius <= 0) || (vertical_radius <= 0))
  {
    throw std::invalid_argument("Figure radius must be positive");
  }
}

bool sogdanov::Figure::contains(const long double x, const long double y) const
{
  const long double center_x = center_x_;
  const long double center_y = center_y_;
  const long double normalized_x = (x - center_x) / horizontal_radius_;
  const long double normalized_y = (y - center_y) / vertical_radius_;

  return (normalized_x * normalized_x + normalized_y * normalized_y) <= 1.0L;
}

sogdanov::Bounds sogdanov::Figure::getBounds() const
{
  const long double horizontal_radius = horizontal_radius_;
  const long double vertical_radius = vertical_radius_;
  const long double center_x = center_x_;
  const long double center_y = center_y_;

  return {center_x - horizontal_radius, center_x + horizontal_radius, center_y - vertical_radius,
      center_y + vertical_radius};
}

long long sogdanov::Figure::getHorizontalRadius() const
{
  return horizontal_radius_;
}

long long sogdanov::Figure::getVerticalRadius() const
{
  return vertical_radius_;
}

long long sogdanov::Figure::getCenterX() const
{
  return center_x_;
}

long long sogdanov::Figure::getCenterY() const
{
  return center_y_;
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
      throw std::invalid_argument("Expected four integers for each figure");
    }

    const long long vertical_radius = (second == 0) ? first : second;

    figures.emplace_back(first, vertical_radius, center_x, center_y);
  }

  return figures;
}
