#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "../common/shapes.hpp"
#include <map>
#include <string>
#include <vector>

namespace muhamadiarov
{

  using Set_t = std::vector< std::string >;

  struct Data
  {
    std::map< std::string, std::size_t > processes;
    std::map< std::string, Shape > shapes;
    std::map< std::string, Set_t > sets;
    std::map< std::string, InfoProcess > calculations;
  };

}

#endif
