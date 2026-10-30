#ifndef COMMANDS_H
#define COMMANDS_H

#include <cstddef>
#include <string>
#include <vector>

namespace vishnevskiy
{
  struct Circle
  {
    double r_, x_, y_;
  };

  struct CircleBoundaries
  {
    double minX_;
    double maxX_;
    double minY_;
    double maxY_;

    double width()
    {
      return maxX_ - minX_;
    }
    double height()
    {
      return maxY_ - minY_;
    }
    double area()
    {
      return width() * height();
    }
  };

  struct Maps
  {
    std::map<std::string, Circle> shapes;
    std::map<std::string, &std::vector<Circle>> sets;
  };

  CircleBoundaries getBoundaries(Circle c);
  CircleBoundaries getSetBoundaries(std::vector<Circle> &circles);
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
