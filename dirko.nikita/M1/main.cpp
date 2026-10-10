#include <iostream>
#include <utility>
#include <string>
#include <stdexcept>
#include <vector>
#include <cstddef>
#include "area.hpp"
#include "shapes.hpp"

int main(int argc, char **argv)
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Invalid parameters\n";
    return 1;
  }
  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;
  try {
    if (argv[1][0] == '-' || argv[2][0] == '-') {
      std::cerr << "Negative arguments\n";
      return 1;
    }
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    if (argc == 4) {
      if (argv[3][0] == '-') {
        std::cerr << "Negative seed\n";
        return 1;
      }
      seed = std::stoull(argv[3]);
    }
  } catch (const std::invalid_argument &) {
    std::cerr << "Not a number in arguments\n";
    return 1;
  } catch (const std::out_of_range &) {
    std::cerr << "Overflow in arguments\n";
    return 1;
  }
  std::vector< dirko::Circle > circles;
  double radius = 0;
  double place_holder = 0;
  double x = 0;
  double y = 0;
  while (std::cin >> radius >> place_holder >> x >> y) {
    circles.push_back(dirko::Circle(radius, dirko::point_t{x, y}));
  }
  if (!std::cin.eof()) {
    std::cerr << "Invalid input\n";
    return 1;
  }
  std::pair< double, double > areas{0.0, 0.0};
  try {
    areas = dirko::area(circles, threads, tries, seed);
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  std::cout << areas.first << ' ' << areas.second << '\n';
}
