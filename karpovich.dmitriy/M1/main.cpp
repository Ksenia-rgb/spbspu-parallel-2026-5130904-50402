#include <algorithm>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace karpovich
{

  struct Point
  {
    double x, y;
  };

  struct Box
  {
    Point max, min;
  };

  class Shape
  {
  public:
    virtual bool isInside(Point p) const noexcept;
    virtual Point getMaxCoordinates() const noexcept;
    virtual Point getMinCoordinates() const noexcept;
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
    Point getMaxCoordinates() const noexcept override
    {
      return {radius + position.x, radius + position.y};
    }
    Point getMinCoordinates() const noexcept override
    {
      return {-radius + position.x, -radius + position.y};
    }
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
    Point getMaxCoordinates() const noexcept override
    {
      return {radius + position.x, second_radius + position.y};
    }
    Point getMinCoordinates() const noexcept override
    {
      return {-radius + position.x, -second_radius + position.y};
    }
  };

  std::pair< size_t, size_t > calculate(const std::vector< Shape > &shapes, Point min, Point max, size_t tests, size_t seed = 0)
  {
    std::default_random_engine gen(seed);
    std::uniform_real_distribution< double > dist_x(min.x, max.x);
    std::uniform_real_distribution< double > dist_y(min.y, max.y);
    size_t hitsIntersection = 0;
    size_t hitsCombination = 0;
    for (size_t i = 0; i < tests; i++) {
      bool isInsideAny = false;
      bool isInsideAll = true;
      Point p{dist_x(gen), dist_y(gen)};
      for (const Shape & shp: shapes) {
        if (shp.isInside(p)) {
          isInsideAny = true;
        } else {
          isInsideAll = false;
        }
      }
      if (isInsideAll) {
        hitsIntersection++;
      }
      if (isInsideAny) {
        hitsCombination++;
      }
    }
    return {hitsIntersection, hitsCombination};
  }

  std::pair< double, double > area(const std::vector< Shape > &shapes, size_t threads, size_t tests)
  {

  }

  Box findBox(const std::vector< Shape > &shapes)
  {
    double inf = std::numeric_limits< double >::infinity();
    Point max{inf, inf};
    Point min{-inf, -inf};
    for (const auto &shape : shapes) {
      min.x = std::min(min.x, shape.getMaxCoordinates().x);
      max.x = std::max(max.x, shape.getMaxCoordinates().x);
      min.y = std::min(min.y, shape.getMaxCoordinates().y);
      max.y = std::max(max.y, shape.getMaxCoordinates().y);
    }
    return {max, min};
  }
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
