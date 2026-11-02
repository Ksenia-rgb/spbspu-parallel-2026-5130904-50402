#include "registry.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include "calculation.hpp"
#include "shapes.hpp"

namespace
{
  template< class Map >
  typename Map::mapped_type& findOrThrow(Map& map, const std::string& name)
  {
    const typename Map::iterator position = map.find(name);
    if (position == map.end())
    {
      throw std::out_of_range("unknown name: " + name);
    }
    return position->second;
  }

  template< class Map >
  const typename Map::mapped_type& findOrThrow(const Map& map, const std::string& name)
  {
    const typename Map::const_iterator position = map.find(name);
    if (position == map.end())
    {
      throw std::out_of_range("unknown name: " + name);
    }
    return position->second;
  }

  template< class Map >
  void insertOrThrow(Map& map, const std::string& name, const typename Map::mapped_type& value)
  {
    if (!pozdnyakov::isValidName(name))
    {
      throw std::invalid_argument("invalid name: " + name);
    }
    if (!map.emplace(name, value).second)
    {
      throw std::invalid_argument("name already exists: " + name);
    }
  }
}

bool pozdnyakov::isValidName(const std::string& name)
{
  return !name.empty() && std::isalpha(static_cast< unsigned char >(name.front()));
}

void pozdnyakov::Registry::addShape(const std::string& name, const ellipse_t& shape)
{
  insertOrThrow(shapes_, name, shape);
}

void pozdnyakov::Registry::addSet(const std::string& name, const names_t& shapes)
{
  const bool all_known = std::all_of(shapes.cbegin(), shapes.cend(),
      [this](const std::string& shape)
      {
        return shapes_.count(shape) != 0;
      });
  if (shapes.empty() || !all_known)
  {
    throw std::invalid_argument("set must consist of existing shapes");
  }
  insertOrThrow(sets_, name, shapes);
}

void pozdnyakov::Registry::addCalculation(const std::string& name, const std::string& process)
{
  const calculation_t calculation{process, calculation_status_t::in_progress, {0.0, 0.0}};
  insertOrThrow(calculations_, name, calculation);
}

bool pozdnyakov::Registry::hasCalculation(const std::string& name) const
{
  return calculations_.count(name) != 0;
}

const pozdnyakov::ellipse_t& pozdnyakov::Registry::getShape(const std::string& name) const
{
  return findOrThrow(shapes_, name);
}

pozdnyakov::ellipses_t pozdnyakov::Registry::getSet(const std::string& name) const
{
  const names_t& names = findOrThrow(sets_, name);
  ellipses_t result;
  result.reserve(names.size());
  for (const std::string& shape : names)
  {
    result.push_back(getShape(shape));
  }
  return result;
}

pozdnyakov::calculation_t& pozdnyakov::Registry::getCalculation(const std::string& name)
{
  return findOrThrow(calculations_, name);
}

const pozdnyakov::calculation_t& pozdnyakov::Registry::getCalculation(const std::string& name) const
{
  return findOrThrow(calculations_, name);
}
