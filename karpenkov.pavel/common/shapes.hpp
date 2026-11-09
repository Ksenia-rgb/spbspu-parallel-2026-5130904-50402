#include <stdexcept>
#include <vector>

namespace karpenkov
{
  struct Circle
  {
  public:
    Circle(int x, int y, int radius):
      x_(x),
      y_(y),
      radius_(radius)
    {
      if (radius <= 0)
      {
        throw std::invalid_argument("Radius must be positive");
      }
    }
    int getRadius() const
    {
      return radius_;
    }
    double left() const
    {
      return static_cast< double >(x_) - radius_;
    }
    double right() const
    {
      return static_cast< double >(x_) + radius_;
    }
    double bottom() const
    {
      return static_cast< double >(y_) - radius_;
    }
    double top() const
    {
      return static_cast< double >(y_) + radius_;
    }
    bool contains(double px, double py) const
    {
      const double dx = px - x_;
      const double dy = py - y_;
      const double r = radius_;
      return dx * dx + dy * dy <= r * r;
    }
  private:
    int x_;
    int y_;
    int radius_;
  };

  struct Box
  {
    double min_x, max_x, min_y, max_y;
    double area() const
    {
      return (max_x - min_x) * (max_y - min_y);
    }
  };

  Box boundingBox(const std::vector< Circle > &circles);
}