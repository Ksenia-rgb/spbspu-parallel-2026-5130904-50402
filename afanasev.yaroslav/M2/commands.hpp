#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include "shape.hpp"
#include "worker.hpp"

namespace afanasev
{
  struct Calc
  {
    std::string proc;
    std::size_t calc_id = 0;
    bool done = false;
    double ms = 0.0;
    double area = 0.0;
  };

  struct Data
  {
    std::map< std::string, Shape > shapes;
    std::map< std::string, std::vector< std::string > > sets;
    std::map< std::string, Worker > workers;
    std::map< std::string, Calc > calcs;
    std::map< std::size_t, std::string > calc_name_by_id;
    std::size_t next_calc_id = 0;
  };

  bool cmdCircle(std::istream &, std::ostream &, Data &);
  bool cmdSet(std::istream &, std::ostream &, Data &);
  bool cmdShow(std::istream &, std::ostream &, Data &);
  bool cmdShowSet(std::istream &, std::ostream &, Data &);
  bool cmdFrame(std::istream &, std::ostream &, Data &);
  bool cmdFrameSet(std::istream &, std::ostream &, Data &);
  bool cmdSpawn(std::istream &, std::ostream &, Data &);
  bool cmdAreaOn(std::istream &, std::ostream &, Data &);
  bool cmdStatus(std::istream &, std::ostream &, Data &);
  bool cmdWait(std::istream &, std::ostream &, Data &);
}

#endif
