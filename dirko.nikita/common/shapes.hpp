#ifndef SHAPES_HPP
#define SHAPES_HPP
namespace dirko {
  struct point_t {
    double x, y;
  };
  struct borders_t {
    dirko::point_t max, min;
  };
  enum class shapeType { CIRCLE, ELLIPSE };
  struct shape_t {
    shapeType type;
    double rx;
    double ry;
    point_t position;
  };
  bool contains(const shape_t &shape, point_t point) noexcept;
  point_t getMinCorner(const shape_t &shape) noexcept;
  point_t getMaxCorner(const shape_t &shape) noexcept;
}
#endif
