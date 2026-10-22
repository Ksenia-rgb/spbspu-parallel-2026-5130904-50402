#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../common/shape.hpp"
#include "worker.hpp"

namespace karpovich
{
  using Set = std::vector< std::string >;

  struct Calc
  {
    std::string proc;
    size_t calc_id = 0;
    enum State
    {
      IN_PROGRESS,
      DONE
    } state = IN_PROGRESS;
    double ms = 0.0;
    double area = 0.0;
  };

  struct Data
  {
    std::map< std::string, std::shared_ptr< Shape > > shapes;
    std::map< std::string, Set > sets;
    std::map< std::string, Worker > workers;
    std::map< std::string, Calc > calcs;
    size_t next_calc_id = 0;
  };

  void cmdCircle(std::istream &, std::ostream &, Data &);
  void cmdSet(std::istream &, std::ostream &, Data &);
  void cmdShow(std::istream &, std::ostream &, Data &);
  void cmdShowSet(std::istream &, std::ostream &, Data &);
  void cmdFrame(std::istream &, std::ostream &, Data &);
  void cmdFrameSet(std::istream &, std::ostream &, Data &);
  void cmdSpawn(std::istream &, std::ostream &, Data &);
  void cmdAreaOn(std::istream &, std::ostream &, Data &);
  void cmdStatus(std::istream &, std::ostream &, Data &);
  void cmdWait(std::istream &, std::ostream &, Data &);
}

#endif
