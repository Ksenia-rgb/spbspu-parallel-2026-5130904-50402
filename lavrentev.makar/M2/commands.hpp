#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <circle.hpp>
#include "process.hpp"

namespace lavrentev
{
  using set_map = std::unordered_map<std::string, std::vector<std::shared_ptr<Circle>>>;
  using circle_data = std::unordered_map<std::string, std::shared_ptr<Circle>>;
  using process_map = std::unordered_map<std::string, std::shared_ptr<Process>>;

  void circle(std::istream &in, std::ostream &, set_map &, circle_data &ac, process_map &pm);
  void set(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &pm);
  void show(std::istream &in, std::ostream &out, set_map &, circle_data &ac, process_map &pm);
  void showset(std::istream &in, std::ostream &out, set_map &sets, circle_data &ac, process_map &pm);
  void frame(std::istream &in, std::ostream &out, set_map &sets, circle_data &ac, process_map &pm);
  void frameset(std::istream &in, std::ostream &out, set_map &sets, circle_data &ac, process_map &pm);
  void spawn(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &pm);
  void area_on(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &pm);
  void status(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &pm);
  void wait(std::istream &in, std::ostream &, set_map &sets, circle_data &ac, process_map &pm);
}

#endif
