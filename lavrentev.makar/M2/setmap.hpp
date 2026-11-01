#ifndef SETMAP_HPP
#define SETMAP_HPP

#include <unordered_map>
#include <vector>
#include <memory>
#include "circle.hpp"

namespace lavrentev
{
  using set_map = std::unordered_map<std::string, std::vector<std::shared_ptr<Circle>>>;
}

#endif
