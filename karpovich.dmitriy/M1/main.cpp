#include <iostream>
#include <stdexcept>
#include <utility>
#include "area.hpp"
#include "shape.hpp"

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
    if (argv[1][0] == '-' || argv[2][0] == '-') {
      std::cerr << "Args must be non-negative\n";
      return 1;
    }
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    if (argc == 4) {
      if (argv[3][0] == '-') {
        std::cerr << "Seed must be non-negative\n";
        return 1;
      }
      seed = std::stoull(argv[3]);
    }
  } catch (const std::invalid_argument &) {
    std::cerr << "All args must be a number\n";
    return 1;
  } catch (const std::out_of_range &) {
    std::cerr << "Args overflow\n";
    return 1;
  }
  std::vector< std::unique_ptr< Shape > > shapes;
  double radius = 0;
  double second_radius = 0;
  double x = 0;
  double y = 0;
  while (std::cin >> radius >> second_radius >> x >> y) {
    if (second_radius) {
      shapes.push_back(std::unique_ptr< Shape >(new Ellipse(radius, second_radius, Point{x, y})));
    } else {
      shapes.push_back(std::unique_ptr< Shape >(new Circle(radius, Point{x, y})));
    }
  }
  if (!std::cin.eof()) {
    std::cerr << "Failed to parse input\n";
    return 1;
  }
  std::pair< double, double > areas{0.0, 0.0};
  try {
    areas = area(shapes, threads, tries, seed);
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  std::cout << areas.first << ' ' << areas.second << '\n';
}
