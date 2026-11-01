#ifndef PROCESSMAP_HPP
#define PROCESSMAP_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "process.hpp"

namespace lavrentev
{
  using process_map = std::unordered_map< std::string, std::unique_ptr< Process > >;
}

#endif
