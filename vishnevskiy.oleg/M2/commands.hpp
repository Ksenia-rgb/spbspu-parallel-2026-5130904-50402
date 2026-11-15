#ifndef COMMANDS_H
#define COMMANDS_H

#include <cstddef>
#include <string>
#include <vector>
#include <unistd.h>
#include <map>
#include <random>

namespace vishnevskiy
{
  struct Circle
  {
    double r_, x_, y_;
  };

  using circ_t = std::vector< Circle >;

  struct CircleBoundaries
  {
    double min_x_;
    double max_x_;
    double min_y_;
    double max_y_;

    double width()
    {
      return max_x_ - min_x_;
    }
    double height()
    {
      return max_y_ - min_y_;
    }
    double area()
    {
      return width() * height();
    }
  };

  struct ProcInfo
  {
    pid_t pid;
    int pipe_to_child;
    int pipe_to_parent;
    size_t seed;
  };

  struct ThreadInfo
  {
    const circ_t *circles;
    const CircleBoundaries *b;
    size_t tests;
    size_t seed;
    size_t result;
  };

  struct CalcInfo
  {
    std::string proc_name;
    bool done = false;
    double area = 0.0;
    double ms = 0.0;
  };

  struct Maps
  {
    std::map< std::string, Circle > shapes;
    std::map< std::string, circ_t > sets;
    std::map< std::string, ProcInfo > procs;
    std::map< std::string, CalcInfo > calcs;
  };

  bool isInside(double x, double y, const Circle &c);
  void generatePoint(std::default_random_engine &gen, const CircleBoundaries &b, double &px, double &py, bool single);
  size_t calc(const circ_t &circles, size_t tests, size_t seed, const CircleBoundaries &b);
  size_t runCalc(const circ_t &circles, size_t threads, size_t tests, size_t seed, const CircleBoundaries &b);
  CircleBoundaries getBoundaries(Circle c);
  CircleBoundaries getSetBoundaries(circ_t &circles);
  void circle(std::istream &i, std::ostream &, Maps &map);
  void set(std::istream &i, std::ostream &, Maps &map);
  void show(std::istream &i, std::ostream &o, Maps &map);
  void showset(std::istream &i, std::ostream &o, Maps &map);
  void frame(std::istream &i, std::ostream &o, Maps &map);
  void frameset(std::istream &i, std::ostream &o, Maps &map);
  void spawn(std::istream &i, std::ostream &, Maps &map);
  void area_on(std::istream &i, std::ostream &o, Maps &map);
  void status(std::istream &i, std::ostream &o, Maps &map);
  void wait(std::istream &i, std::ostream &o, Maps &map);
}

#endif
