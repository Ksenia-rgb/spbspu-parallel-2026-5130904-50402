#include "catalog.hpp"
#include <stdexcept>
#include <cmath>
#include "parse.hpp"

void matveev::Catalog::addShape(const std::string &name, const shape_t &shape)
{
  requireName(name);
  if ((shape.radiusX <= 0) || (shape.radiusY <= 0) || !std::isfinite(shape.angle))
  {
    throw std::invalid_argument("Invalid shape");
  }
  if (!shapes_.emplace(name, shape).second)
  {
    throw std::invalid_argument("Shape already exists");
  }
}

const matveev::shape_t &matveev::Catalog::getShape(const std::string &name) const
{
  return shapes_.at(name);
}

void matveev::Catalog::addSet(const std::string &name, const std::vector< std::string > &names)
{
  requireName(name);
  if (names.empty())
  {
    throw std::invalid_argument("Empty set");
  }
  for (const std::string &shape: names)
  {
    getShape(shape);
  }
  if (!sets_.emplace(name, names).second)
  {
    throw std::invalid_argument("Set already exists");
  }
}

std::vector< matveev::shape_t > matveev::Catalog::getSet(const std::string &name) const
{
  const std::vector< std::string > &names = sets_.at(name);
  std::vector< shape_t > shapes = {};
  shapes.reserve(names.size());
  for (const std::string &shape: names)
  {
    shapes.push_back(getShape(shape));
  }
  return shapes;
}
