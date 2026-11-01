#ifndef CIRCLEDATA_HPP
#define CIRCLEDATA_HPP

#include <unordered_map>
#include <memory>
#include "circle.hpp"

namespace lavrentev
{
  using circle_data = std::unordered_map<std::string, std::shared_ptr<Circle>>;
}

#endif
