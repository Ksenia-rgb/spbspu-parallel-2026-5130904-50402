#ifndef COMMANDS_H
#define COMMANDS_H

#include <cstddef>
#include <string>
#include <vector>

namespace vishnevskiy
{
  struct Circle
  {
    double r, x, y;
  };

  struct CircleBoundaries
  {
    double minX;
    double maxX;
    double minY;
    double maxY;

    double width()
    {
      return maxX - minX;
    }
    double height()
    {
      return maxY - minY;
    }
    double area()
    {
      return width() * height();
    }
  };

  struct Maps
  {
    std::map<std::string, Circle> shapes;
    std::map<std::string, std::vector<std::string>> set;
  };

  void circle(std::istream &, std::ostream &, Maps &);
  void set(std::istream &, std::ostream &, Maps &);
  void show(std::istream &, std::ostream &, Maps &);
  void showset(std::istream &, std::ostream &, Maps &);
  void frame(std::istream &, std::ostream &, Maps &);
  void frameset(std::istream &, std::ostream &, Maps &);
  void spawn(std::istream &, std::ostream &, Maps &);
  void area_on(std::istream &, std::ostream &, Maps &);
  void status(std::istream &, std::ostream &, Maps &);
  void wait(std::istream &, std::ostream &, Maps &);
}

#define
