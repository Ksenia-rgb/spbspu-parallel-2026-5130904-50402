#ifndef CALCMAP_HPP
#define CALCMAP_HPP

#include <string>
#include <unordered_map>

namespace lavrentev
{
  struct CalcRes
  {
    std::string process_name;
    double result;
    double time;
  };

  using calc_map = std::unordered_map< std::string, CalcRes >;
}

#endif
