#include <cctype>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include "../common/ellipse.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include "constants.hpp"
#include "../common/area.hpp"

namespace shirokov
{
  size_t stringToSizeT(const char* line);
}

int main(int argc, const char* argv[])
{
  if (argc < shirokov::count_of_args_without_seed)
  {
    std::cerr << "Not enough arguments" << '\n';
    return shirokov::status_error_invalid_args;
  }

  size_t threads = 0, tries = 0, seed = 0;
  try
  {
    threads = shirokov::stringToSizeT(argv[shirokov::threads_arg_pos]);
    tries = shirokov::stringToSizeT(argv[shirokov::tries_arg_pos]);
    if (!tries)
    {
      std::cerr << "number of tries is not positive" << '\n';
      return shirokov::status_error_invalid_args;
    }
    if (argc == shirokov::count_of_args_with_seed)
    {
      seed = shirokov::stringToSizeT(argv[shirokov::seed_arg_pos]);
    }
  }
  catch (const std::invalid_argument& e)
  {
    std::cerr << e.what() << '\n';
    return shirokov::status_error_invalid_args;
  }

  shirokov::ellipse_list ellipses;
  long long hr = 0, vr = 0, x = 0, y = 0;
  while (std::cin >> hr >> vr >> x >> y)
  {
    if (hr < 0 || vr < 0)
    {
      std::cerr << "Negative radius" << '\n';
      return shirokov::status_error_invalid_input;
    }
    if (!vr)
    {
      vr = hr;
    }
    ellipses.push_back(
        {static_cast< double >(x), static_cast< double >(y), static_cast< double >(hr), static_cast< double >(vr)});
  }
  if (!std::cin.eof() || ellipses.empty())
  {
    std::cerr << "Input error" << '\n';
    return shirokov::status_error_invalid_input;
  }

  const std::pair< double, double > res = shirokov::area(ellipses, threads, tries, seed);
  std::cout << std::fixed << std::setprecision(std::numeric_limits< double >::max_digits10);
  std::cout << res.first << ' ' << res.second << '\n';

  return shirokov::status_ok;
}

size_t shirokov::stringToSizeT(const char* str)
{
  if (!str)
  {
    throw std::invalid_argument("null pointer was passed");
  }

  while (std::isspace(static_cast< unsigned char >(*str)))
  {
    ++str;
  }

  if (*str == '-')
  {
    throw std::invalid_argument("negative number");
  }

  char* end = nullptr;
  errno = 0;
  constexpr int base = 10;
  const unsigned long long res = std::strtoull(str, &end, base);
  if (end == str)
  {
    throw std::invalid_argument("not number");
  }
  if (errno == ERANGE || res > std::numeric_limits< size_t >::max())
  {
    throw std::invalid_argument("overflow");
  }

  return static_cast< size_t >(res);
}
