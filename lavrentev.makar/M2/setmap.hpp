#ifndef SETMAP_HPP
#define SETMAP_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <circle.hpp>

namespace lavrentev
{
  using set_map = std::unordered_map< std::string, std::vector< std::shared_ptr< Circle > > >;
}

#endif
