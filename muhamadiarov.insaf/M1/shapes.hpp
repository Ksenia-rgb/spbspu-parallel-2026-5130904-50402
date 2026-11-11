#ifndef SHAPES_HPP
#define SHAPES_HPP

namespace muhamadiarov
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
    virtual bool isInside(Point) const noexcept = 0;
    virtual double getMaxX() const noexcept = 0;
    virtual double getMinX() const noexcept = 0;
    virtual double getMaxY() const noexcept = 0;
    virtual double getMinY() const noexcept = 0;
  };
  class Circle final: public Shape
  {
  public:
    Circle(double r, Point p) noexcept;
    bool isInside(Point p) const noexcept override;
    double getMaxX() const noexcept override;
    double getMinX() const noexcept override;
    double getMaxY() const noexcept override;
    double getMinY() const noexcept override;

  private:
    double r_;
    Point p_;
  };
  class Ellipse final: public Shape
  {
  public:
    Ellipse(double hr, double vr, Point p) noexcept;
    bool isInside(Point p) const noexcept override;
    double getMaxX() const noexcept override;
    double getMinX() const noexcept override;
    double getMaxY() const noexcept override;
    double getMinY() const noexcept override;
  private:
    double hr_;
    double vr_;
    Point p_;
  };
}
#endif
