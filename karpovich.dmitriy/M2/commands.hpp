#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../common/shape.hpp"

namespace karpovich
{
  using Set = std::vector< std::string >;

  struct Data
  {
    std::map< std::string, std::shared_ptr< Shape > > shapes;
    std::map< std::string, Set > sets;
  };

  void cmdCircle(std::istream &, std::ostream &, Data &);
  void cmdSet(std::istream &, std::ostream &, Data &);
  void cmdShow(std::istream &, std::ostream &, Data &);
  void cmdShowSet(std::istream &, std::ostream &, Data &);
  void cmdFrame(std::istream &, std::ostream &, Data &);
  void cmdFrameSet(std::istream &, std::ostream &, Data &);
}

#endif
