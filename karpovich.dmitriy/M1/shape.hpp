#ifndef SHAPE_HPP
#define SHAPE_HPP

namespace karpovich
{
  struct Point
  {
    double x;
    double y;
  };

  class Shape
  {
  public:
    virtual ~Shape() = default;
    virtual bool contains(Point point) const noexcept = 0;
    virtual Point getMinCorner() const noexcept = 0;
    virtual Point getMaxCorner() const noexcept = 0;
  };

  class Circle final: public Shape
  {
  public:
    Circle(double radius, Point center) noexcept;
    bool contains(Point point) const noexcept override;
    Point getMinCorner() const noexcept override;
    Point getMaxCorner() const noexcept override;

  private:
    double radius_;
    Point center_;
  };

  class Ellipse final: public Shape
  {
  public:
    Ellipse(double horizontal_radius, double vertical_radius, Point center) noexcept;
    bool contains(Point point) const noexcept override;
    Point getMinCorner() const noexcept override;
    Point getMaxCorner() const noexcept override;

  private:
    double horizontal_radius_;
    double vertical_radius_;
    Point center_;
  };

}

#endif
