#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <future>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "shape.hpp"

namespace karpovich
{

  std::pair< size_t, size_t > calculate(const std::vector< Shape > &shapes, Point max, Point min, size_t tests,
                                        size_t seed)
  {
    std::default_random_engine gen(seed);
    std::uniform_real_distribution< double > dist_x(min.x, max.x);
    std::uniform_real_distribution< double > dist_y(min.y, max.y);
    size_t hitsIntersection = 0;
    size_t hitsCover = 0;
    for (size_t i = 0; i < tests; i++) {
      bool isInsideAny = false;
      bool isInsideAll = true;
      Point p{dist_x(gen), dist_y(gen)};
      for (const Shape &shp : shapes) {
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
        hitsCover++;
      }
    }
    return {hitsIntersection, hitsCover};
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

  std::pair< double, double > area(const std::vector< Shape > &shapes, size_t threads, size_t tests, size_t seed = 0)
  {
    if (!threads || !tests) {
      throw std::invalid_argument("args must be > 0");
    }
    std::vector< std::future< std::pair< size_t, size_t > > > futures;
    size_t tests_per_thread = tests / threads;
    size_t remainder = tests % threads;
    Box box = findBox(shapes);
    for (size_t i = 0; i < threads; i++) {
      size_t test_for_task = i < remainder ? tests_per_thread + 1 : tests_per_thread;
      futures.push_back(std::async(std::launch::async, calculate, shapes, box.max, box.min, test_for_task, seed));
    }
    size_t inters = 0;
    size_t covers = 0;
    for (std::future< std::pair< size_t, size_t > > &future : futures) {
      std::pair< size_t, size_t > result = future.get();
      inters += result.first;
      covers += result.second;
    }
    double box_area = std::abs(box.max.x - box.min.x) * std::abs(box.max.y - box.min.y);

    return {box_area * (static_cast< double >(inters) / tests), box_area * (static_cast< double >(covers) / tests)};
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
  std::pair< double, double > areas{0.0, 0.0};
  try {
    areas = area(shapes, threads, tries);
  } catch (...) {
    return 1;
  }
  std::cout << areas.first << ' ' << areas.second << '\n';
}
