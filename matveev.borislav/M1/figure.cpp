#include "figure.hpp"
#include <algorithm>
#include <cstddef>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
  static long long parseInteger(const std::string& token)
  {
    std::size_t position = 0;
    const long long value = std::stoll(token, &position);
    if (position != token.size())
    {
      throw std::invalid_argument("Invalid figure parameter");
    }
    return value;
  }

  static long long readInteger(std::istream& input)
  {
    std::string token{};
    if (!(input >> token))
    {
      throw std::invalid_argument("Cannot read a complete figure");
    }
    return parseInteger(token);
  }
}

std::vector< matveev::figure_t > matveev::readFigures(std::istream& input)
{
  std::vector< figure_t > figures{};
  std::string token{};
  while (input >> token)
  {
    const long long first = parseInteger(token);
    const long long second = readInteger(input);
    const long long x = readInteger(input);
    const long long y = readInteger(input);
    if ((first <= 0) || (second < 0))
    {
      throw std::invalid_argument("Figure semiaxes must be positive (use zero for a circle)");
    }
    const long double horizontal = static_cast< long double >(first);
    const long double vertical = (second == 0) ? horizontal : static_cast< long double >(second);
    figures.push_back({horizontal, vertical, static_cast< long double >(x), static_cast< long double >(y)});
  }
  if (input.bad() || !input.eof())
  {
    throw std::runtime_error("Failed to read figures");
  }
  return figures;
}

bool matveev::isInside(const figure_t& figure, const long double x, const long double y)
{
  const long double dx = (x - figure.center_x) / figure.horizontal_radius;
  const long double dy = (y - figure.center_y) / figure.vertical_radius;
  constexpr long double unit_radius_squared = 1.0L;
  return ((dx * dx) + (dy * dy)) <= unit_radius_squared;
}

matveev::bounds_t matveev::findBounds(const std::vector< figure_t >& figures)
{
  if (figures.empty())
  {
    throw std::invalid_argument("Cannot find bounds of an empty set of figures");
  }
  const figure_t& first = figures.front();
  bounds_t bounds{first.center_x - first.horizontal_radius, first.center_x + first.horizontal_radius,
      first.center_y - first.vertical_radius, first.center_y + first.vertical_radius};
  for (const figure_t& figure : figures)
  {
    bounds.min_x = std::min(bounds.min_x, figure.center_x - figure.horizontal_radius);
    bounds.max_x = std::max(bounds.max_x, figure.center_x + figure.horizontal_radius);
    bounds.min_y = std::min(bounds.min_y, figure.center_y - figure.vertical_radius);
    bounds.max_y = std::max(bounds.max_y, figure.center_y + figure.vertical_radius);
  }
  return bounds;
}
