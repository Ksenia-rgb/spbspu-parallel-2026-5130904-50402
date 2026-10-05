#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace karpovich {

struct Point
{
  size_t x, y;
};

class Shape
{
public:
  bool virtual isInside() const noexcept;
};

struct Circle : Shape
{
  size_t radius;
  Point position;
  Circle(size_t radius, Point position):
    radius(radius),
    position(position)
  {}
};

struct Ellipse : Shape
{
  size_t radius, second_radius;
  Point position;
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
  size_t radius = 0;
  size_t second_radius = 0;
  size_t x = 0;
  size_t y = 0;
  while (std::cin >> radius) {
    std::cin >> second_radius >> x >> y;
    if (second_radius) {
      shapes.push_back(Ellipse(radius, second_radius, Point{x, y}));
      continue;
    }
    shapes.push_back(Circle(radius, Point{x, y}));
  }  
}
