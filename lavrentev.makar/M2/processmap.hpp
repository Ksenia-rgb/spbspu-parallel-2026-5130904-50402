#ifndef PROCESSMAP_HPP
#define PROCESSMAP_HPP

#include <unordered_map>
#include <memory>
#include "process.hpp"

namespace lavrentev
{
  using process_map = std::unordered_map<std::string, std::unique_ptr<Process>>;
}

#endif
