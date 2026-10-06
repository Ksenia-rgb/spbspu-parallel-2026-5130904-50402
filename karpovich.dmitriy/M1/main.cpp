#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "area.hpp"
#include "shape.hpp"

namespace
{
  constexpr int argc_without_seed = 3;
  constexpr int argc_with_seed = 4;
  constexpr int arg_threads = 1;
  constexpr int arg_tries = 2;
  constexpr int arg_seed = 3;
}

int main(int argc, char **argv)
{
  using namespace karpovich;
  if (argc != argc_without_seed && argc != argc_with_seed)
  {
    std::cerr << "Invalid num of args\n";
    return 1;
  }
  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;
  try
  {
    if (argv[arg_threads][0] == '-' || argv[arg_tries][0] == '-')
    {
      std::cerr << "Args must be non-negative\n";
      return 1;
    }
    threads = std::stoull(argv[arg_threads]);
    tries = std::stoull(argv[arg_tries]);
    if (argc == argc_with_seed)
    {
      if (argv[arg_seed][0] == '-')
      {
        std::cerr << "Seed must be non-negative\n";
        return 1;
      }
      seed = std::stoull(argv[arg_seed]);
    }
  }
  catch (const std::invalid_argument &)
  {
    std::cerr << "All args must be a number\n";
    return 1;
  }
  catch (const std::out_of_range &)
  {
    std::cerr << "Args overflow\n";
    return 1;
  }
  std::vector< std::unique_ptr< Shape > > shapes;
  double radius = 0;
  double second_radius = 0;
  double x = 0;
  double y = 0;
  while (std::cin >> radius >> second_radius >> x >> y)
  {
    if (second_radius)
    {
      shapes.push_back(std::unique_ptr< Shape >(new Ellipse(radius, second_radius, Point{x, y})));
    }
    else
    {
      shapes.push_back(std::unique_ptr< Shape >(new Circle(radius, Point{x, y})));
    }
  }
  if (!std::cin.eof())
  {
    std::cerr << "Failed to parse input\n";
    return 1;
  }
  std::pair< double, double > areas{0.0, 0.0};
  try
  {
    areas = area(shapes, threads, tries, seed);
  }
  catch (const std::invalid_argument &e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }
  std::cout << areas.first << ' ' << areas.second << '\n';
  return 0;
}
