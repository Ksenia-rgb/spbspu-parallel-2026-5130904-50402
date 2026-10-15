#ifndef SHAPES_HPP
#define SHAPES_HPP
#include <cstddef>
namespace dirko {
  struct point_t {
    double x, y;
  };
  class Shape {
  public:
    virtual ~Shape() = default;
    virtual bool isInside(point_t) const noexcept = 0;
    virtual point_t getMinCorner() const noexcept = 0;
    virtual point_t getMaxCorner() const noexcept = 0;
  };
  class Circle final: public Shape {
  public:
    Circle(size_t, point_t);
    bool isInside(point_t) const noexcept;
    point_t getMaxCorner() const noexcept;
    point_t getMinCorner() const noexcept;
  private:
    double radius_;
    point_t position_;
  };
  class Ellipse final: public Shape {
  public:
    Ellipse(double, double, point_t) noexcept;
    bool isInside(point_t) const noexcept override;
    point_t getMinCorner() const noexcept override;
    point_t getMaxCorner() const noexcept override;

  private:
    double horizontal_radius_;
    double vertical_radius_;
    point_t position_;
  };
}
#endif
