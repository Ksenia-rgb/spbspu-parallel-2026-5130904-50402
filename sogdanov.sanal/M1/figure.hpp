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

  private:
    long double horizontal_radius_;
    long double vertical_radius_;
    long double center_x_;
    long double center_y_;
  };

  std::vector< Figure > readFigures(std::istream& input);
}

#endif
