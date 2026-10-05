#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace karpovich
{

  struct Point
  {
    double x, y;
  };

  class Shape
  {
  public:
    bool virtual isInside(Point p) const noexcept;
  };

  struct Circle: Shape
  {
    double radius;
    Point position;
    bool isInside(Point p) const noexcept override
    {
      return (p.x - position.x) * (p.x - position.x) + (p.y - position.y) * (p.y - position.y) <= radius * radius;
    }
    Circle(size_t radius, Point position):
      radius(radius),
      position(position)
    {}
  };

  struct Ellipse: Shape
  {
    double radius, second_radius;
    Point position;
    bool isInside(Point p) const noexcept override
    {
      double dx = p.x - position.x;
      double dy = p.y - position.y;
      return (dx * dx * second_radius * second_radius + dy * dy * radius * radius) <= (radius * radius * second_radius * second_radius);
    }
    Ellipse(size_t r, size_t s_r, Point pos):
      radius(r),
      second_radius(s_r),
      position(pos)
    {}
  };

}

int main(int argc, char **argv)
{
  using namespace karpovich;
  if (argc != 3 && argc != 4) {
    std::cerr << "Invalid num of args\n";
    return 1;
  }
  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;
  try {
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    if (argc == 4) {
      seed = std::stoull(argv[3]);
    }
  } catch (const std::invalid_argument &) {
    std::cerr << "All args must be a number\n";
    return 1;
  } catch (const std::out_of_range &) {
    std::cerr << "Args overflow\n";
    return 1;
  }
  std::vector< Shape > shapes;
  double radius = 0;
  double second_radius = 0;
  double x = 0;
  double y = 0;
  while (std::cin >> radius) {
    std::cin >> second_radius >> x >> y;
    if (second_radius) {
      shapes.push_back(Ellipse(radius, second_radius, Point{x, y}));
      continue;
    }
    shapes.push_back(Circle(radius, Point{x, y}));
  }
}
