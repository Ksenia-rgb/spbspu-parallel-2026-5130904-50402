#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <iostream>
#include <circle.hpp>
#include "setmap.hpp"
#include "circledata.hpp"
#include "processmap.hpp"
#include "calcmap.hpp"

namespace lavrentev
{
  struct Data
  {
    set_map &sets;
    circle_data &ac;
    process_map &pm;
    calc_map &cm;
  };

  void circle(std::istream &in, std::ostream &, Data &data);
  void set(std::istream &in, std::ostream &, Data &data);
  void show(std::istream &in, std::ostream &out, Data &data);
  void showset(std::istream &in, std::ostream &out, Data &data);
  void frame(std::istream &in, std::ostream &out, Data &data);
  void frameset(std::istream &in, std::ostream &out, Data &data);
  void spawn(std::istream &in, std::ostream &, Data &data);
  void area_on(std::istream &in, std::ostream &, Data &data);
  void status(std::istream &in, std::ostream &, Data &data);
  void wait(std::istream &in, std::ostream &, Data &data);
}

#endif
