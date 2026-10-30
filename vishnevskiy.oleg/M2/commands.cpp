#include <iostream>
#include <string>
#include <map>
#include "shapes.hpp"

namespace vishnevskiy
{
  CircleBoundaries getBoundaries(Circle c)
  {
    CircleBoundaries b {0,0,0,0};
    b.minX_ = c.x - c.r;
    b.maxX_ = c.x + c.r;
    b.minY_ = c.y - c.r;
    b.maxY_ = c.y + c.r;
    return b;
  }

  CircleBoundaries getSetBoundaries(std::vector<Circle> &circles)
  {
    CircleBoundaries b
    {
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest(),
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest()
    };
    for (size_t i = 0; i < circles.size(); ++i)
    {
      const Circle& c = circles[i];
      double curMinX = c.x - c.r;
      double curMaxX = c.x + c.r;
      double curMinY = c.y - c.r;
      double curMaxY = c.y + c.r;
      if (curMinX < b.minX_)
      {
        b.minX_ = curMinX;
      }
      if (curMaxX > b.maxX_)
      {
        b.maxX_ = curMaxX;
      }
      if (curMinY < b.minY_)
      {
        b.minY_ = curMinY;
      }
      if (curMaxY > b.maxY_)
      {
        b.maxY_ = curMaxY;
      }
    }
    return b;
  }

  void circle(std::istream &i, std::ostream &, Maps &map)
  {
    std::string name;
    double r, x, y;
    if (!(i >> name >> r >> x >> y))
    {
      throw std::runtime_error("Invalid arguments");
    }
    if (map.shapes -> count(name))
    {
      throw std::runtime_error("Invalid name");
    }
    Circle res = {r_ = r, x_ = x, y_ = y};
    map.shapes[name] = res;
  }

  void set(std::istream &i, std::ostream &, Maps &map)
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
    map.sets[name] = &shapeSet;
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
    o << res.r_ << res.x_ << res.y_ << "\n";
  }

  void showset(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      std::vector<Circle> &shapeSet = map.shapes -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    for (size_t i = 0; i < shapeSet.size(); ++i)
    {
      Circle res = shapeSet -> at(i);
      o << res.r_ << res.x_ << res.y_ << "\n";
    }
  }

  void frame(std::istream &i, std::ostream &o, Maps &map)
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
    CircleBoundaries b = getBoundaries(res);
    o << b.maxX_ << b.maxY_ << b.minX_ << b.minY_ << "\n";
  }

  void frame(std::istream &i, std::ostream &o, Maps &map)
  {
    Circle res;
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      std::vector<Circle> &shapeSet = map.sets -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    CircleBoundaries b = getSetBoundaries(shapeSet);
    o << b.maxX_ << b.maxY_ << b.minX_ << b.minY_ << "\n";
  }
}

