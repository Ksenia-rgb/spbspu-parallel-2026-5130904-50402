#ifndef SHAPE_HPP
#define SHAPE_HPP

namespace karpovich
{
  struct point_t
  {
    double x;
    double y;
  };

  struct rectangle_t
  {
    point_t max;
    point_t min;
  };

  class Shape
  {
  public:
    virtual ~Shape() = default;
    virtual bool contains(point_t point) const noexcept = 0;
    virtual point_t getMinCorner() const noexcept = 0;
    virtual point_t getMaxCorner() const noexcept = 0;
  };

  class Circle final: public Shape
  {
  public:
    Circle(double radius, point_t center) noexcept;
    bool contains(point_t point) const noexcept override;
    point_t getMinCorner() const noexcept override;
    point_t getMaxCorner() const noexcept override;
    double getRadius() const noexcept;
    point_t getCenter() const noexcept;

  private:
    double radius_;
    point_t center_;
  };

  class Ellipse final: public Shape
  {
  public:
    Ellipse(double horizontal_radius, double vertical_radius, point_t center) noexcept;
    bool contains(point_t point) const noexcept override;
    point_t getMinCorner() const noexcept override;
    point_t getMaxCorner() const noexcept override;
    double getHorizontalRadius() const noexcept;
    double getVerticalRadius() const noexcept;
    point_t getCenter() const noexcept;

  private:
    double horizontal_radius_;
    double vertical_radius_;
    point_t center_;
  };

}

#endif
