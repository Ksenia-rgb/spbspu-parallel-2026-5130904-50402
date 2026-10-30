#include <iostream>
#include <string>
#include <map>
#include "shapes.hpp"

namespace vishnevskiy
{
  void circle(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    double r, x, y;
    if (!(i >> name >> r >> x >> y))
    {
      throw std::runtime_error("Invalid arguments");
    }
    if (map.sets -> count(name))
    {
      throw std::runtime_error("Invalid name");
    }
    Circle res = {r_ = r, x_ = x, y_ = y};
    map.shapes[name] = res;
  }

  void set(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    size_t size;
    if (!(i >> name >> size) || size == 0)
    {
      throw std::runtime_error("Invalid arguments");
    }
    if (map.sets -> count(name))
    {
      throw std::runtime_error("Invalid name");
    }
    std::vector<Circle> shapeSet;
    shapeSet.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
      std::string shapeName;
      Circle el;
      i >> shapeName;
      try
      {
        el = map.shapes -> at(shapeName);
      }
      catch(const std::out_of_range& e)
      {
        throw std::out_of_range("No name found");
      }
      shapeSet.push_back(el);
    }
    map.sets[name] = shapeSet;
  }

  void show(std::istream &i, std::ostream &o, Maps &map)
  {
    Circle res;
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      res = map.shapes -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    o << res.r_ << res.x_ << res.y_;
  }
}

