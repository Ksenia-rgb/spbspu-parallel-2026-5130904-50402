#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <iosfwd>
#include <vector>

namespace sogdanov
{
  struct Bounds
  {
    long double min_x;
    long double max_x;
    long double min_y;
    long double max_y;
  };

  class Figure
  {
  public:
    Figure(long long horizontal_radius, long long vertical_radius, long long center_x, long long center_y);

    bool contains(long double x, long double y) const;
    Bounds getBounds() const;

    long long getHorizontalRadius() const;
    long long getVerticalRadius() const;
    long long getCenterX() const;
    long long getCenterY() const;

  private:
    long long horizontal_radius_;
    long long vertical_radius_;
    long long center_x_;
    long long center_y_;
  };

  std::vector< Figure > readFigures(std::istream& input);
}

#endif
