#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "process.hpp"
#include "../common/shapes.hpp"
#include <cstddef>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <istream>
#include <ostream>

namespace muhamadiarov
{

  using Set_t = std::vector< std::string >;

  struct Data
  {
    std::map< std::string, std::unique_ptr< Shape > > shapes;
    std::map< std::string, Set_t > sets;
  };

  void cmdCircle(std::istream&, std::ostream&, Data&);
  void cmdSet(std::istream&, std::ostream&, Data&);
  void cmdShow(std::istream&, std::ostream&, Data&);
  void cmdShowSet(std::istream&, std::ostream&, Data&);
  void cmdFrame(std::istream&, std::ostream&, Data&);
  void cmdFrameSet(std::istream&, std::ostream&, Data&);
  void cmdSpawn(std::istream&, std::ostream&, Data&);
  void cmdAreaOn(std::istream&, std::ostream&, Data&);
  void cmdStatus(std::istream&, std::ostream&, Data&);
  void cmdWait(std::istream&, std::ostream&, Data&);

}

#endif
