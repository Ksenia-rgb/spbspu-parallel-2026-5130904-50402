#ifndef CALCMAP_HPP
#define CALCMAP_HPP

#include <unordered_map>
#include <string>

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
