#include "monte_carlo.hpp"
#include "shapes.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

int main(int argc, char *argv[])
{
  if (argc != 3 && argc != 4)
  {
    std::cerr << "insufficient number of arguments\n";
    return 1;
  }
  if (argv[1][0] == '-' || argv[2][0] == '-')
  {
    std::cerr << "arguments must be non-negative numbers\n";
    return 1;
  }
  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;
  try
  {
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    if (tries == 0)
    {
      std::cerr << "the number of tries must be positive\n";
      return 1;
    }
    if (argc == 4)
    {
      if (argv[3][0] == '-')
      {
        std::cerr << "arguments must be non-negative numbers\n";
        return 1;
      }
      seed = std::stoull(argv[3]);
      if (!seed)
      {
        std::cerr << "number of trials must be positive\n";
        return 1;
      }
    }
  }
  catch (const std::invalid_argument &)
  {
    std::cerr << "arguments aren't numbers\n";
    return 1;
  }
  catch (const std::out_of_range &)
  {
    std::cerr << "numberical values of arguments exceed the permissible limit\n";
    return 1;
  }
  namespace muh = muhamadiarov;
  std::vector< std::unique_ptr< muh::Shape > > shapes;
  double hr = 0;
  int vr = 0;
  double x = 0;
  double y = 0;
  while (std::cin >> hr >> vr >> x >> y)
  {
    if (vr)
    {
      shapes.push_back(std::make_unique< muh::Ellipse >(hr, vr, muh::Point{x, y}));
    }
    else
    {
      shapes.push_back(std::make_unique< muh::Circle >(hr, muh::Point{x, y}));
    }
  }
  if (!std::cin.eof() && std::cin.fail())
  {
    std::cerr << "invalid input\n";
    return 1;
  }
  if (shapes.empty())
  {
    std::cout << 0.0 << ' ' << 0.0 << '\n';
    return 0;
  }
  const std::pair< double, double > results = getAreas(shapes, threads, tries, seed);
  std::cout << results.first << ' ' << results.second << '\n';
  return 0;
}
