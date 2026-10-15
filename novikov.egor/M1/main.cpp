#include "circle.hpp"
#include "operations.hpp"
#include "parse.hpp"
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    std::cerr << "Invalid quantity of arguments\n";
    return 1;
  }

  std::size_t threads = 1;
  std::size_t tries = 0;
  std::size_t seed = 0;

  try
  {
    threads = novikov::stringToSizeT(argv[1]);
    tries = novikov::stringToSizeT(argv[2]);
    if (argc == 4)
    {
      seed = novikov::stringToSizeT(argv[3]);
    }
  }
  catch (const std::invalid_argument& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }

  if (tries == 0)
  {
    std::cerr << "Tries can't be zero\n";
    return 1;
  }

  if (threads == 0)
  {
    threads = 1;
  }

  std::vector< novikov::Circle > circles;
  long long r = 0;
  long long dummy = 0;
  long long x = 0;
  long long y = 0;

  while (std::cin >> r >> dummy >> x >> y)
  {
    if (r <= 0)
    {
      std::cerr << "Radius must be positive\n";
      return 2;
    }
    circles.push_back(novikov::Circle(static_cast< double >(r), static_cast< double >(x), static_cast< double >(y)));
  }

  if (!std::cin.eof())
  {
    std::cerr << "Input error\n";
    return 2;
  }

  if (circles.empty())
  {
    std::cerr << "No circles\n";
    return 2;
  }

  const std::pair< double, double > res = novikov::area(circles, threads, tries, seed);

  std::cout << std::setprecision(std::numeric_limits< double >::max_digits10) << res.first << ' ' << res.second << '\n';

  return 0;
}
