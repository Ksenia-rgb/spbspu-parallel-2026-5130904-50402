#include <iostream>
#include <string>
#include <map>
#include "shapes.hpp"

namespace vishnevskiy
{
  void circle(std::istream &i, std::ostream &o, Maps &map)
  {
    Circle res;
    std::string name;
    double r, x, y;
    if (!(i >> name >> r >> x >> y))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      res = map.shapes -> at(name);
      o << res.r << res.x << res.y;
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
  }
}

