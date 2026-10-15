#ifndef INPUT_HPP
#define INPUT_HPP

#include <iosfwd>
#include "shape.hpp"

namespace samarin {
  shapes_t readShapes(std::istream & in);
}

#endif
