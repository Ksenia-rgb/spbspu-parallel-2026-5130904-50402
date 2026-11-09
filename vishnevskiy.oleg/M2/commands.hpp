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

  struct ProcInfo
  {
    pid_t pid;
    int pipeToChild;
    int pipeToParent;
    size_t seed;
  };

  struct ThreadInfo
  {
    const std::vector<Circle> *circles;
    const CircleBoundaries *b;
    size_t tests;
    size_t seed;
    size_t result;
  };

  struct CalcInfo
  {
    std::string procName;
    bool done = false;
    double area = 0.0;
    double ms = 0.0;
  };

  struct Maps
  {
    std::map<std::string, Circle> shapes;
    std::map<std::string, std::vector<Circle>> sets;
    std::map<std::string, ProcInfo> procs;
    std::map<std::string, CalcInfo> calcs;
  };

  bool isInside(double x, double y, const Circle &c);
  void generatePoint(std::mt19937_64 &gen, const CircleBoundaries &b, double &px, double &py);
  size_t calc(const std::vector<Circle> &circles, size_t tests, size_t seed, const CircleBoundaries &b);
  size_t runCalc(const std::vector<Circle> &circles, size_t threads, size_t tests, size_t seed, const CircleBoundaries &b);
  CircleBoundaries getBoundaries(Circle c);
  CircleBoundaries getSetBoundaries(std::vector<Circle> &circles);
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
