#include "input.hpp"
#include <istream>
#include <memory>
#include <stdexcept>
#include "circle.hpp"
#include "ellipse.hpp"

samarin::shapes_t samarin::readShapes(std::istream & in)
{
  shapes_t shapes;
  int horizontal = 0;
  while (in >> horizontal) {
    int vertical = 0;
    int x = 0;
    int y = 0;
    if (!(in >> vertical >> x >> y)) {
      throw std::invalid_argument("Invalid shape parameters");
    }
    const point_t center{static_cast< double >(x), static_cast< double >(y)};
    if (vertical == 0) {
      shapes.push_back(std::make_unique< Circle >(horizontal, center));
    } else {
      shapes.push_back(std::make_unique< Ellipse >(horizontal, vertical, center));
    }
  }
  if (!in.eof()) {
    throw std::invalid_argument("Invalid shape parameters");
  }
  return shapes;
}
