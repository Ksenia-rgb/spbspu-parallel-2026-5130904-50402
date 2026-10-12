#include <cctype>
#include <cerrno>
#include <future>
#include <iomanip>
#include <cstddef>
#include <utility>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <functional>

namespace shirokov
{
  struct Ellipse;
  using ellipse_list = std::vector< Ellipse >;

  constexpr int status_ok = 0;
  constexpr int status_error_invalid_args = 1;
  constexpr int status_error_invalid_input = 2;

  constexpr int count_of_args_without_seed = 3;
  constexpr int count_of_args_with_seed = 4;

  constexpr size_t threads_arg_pos = 1;
  constexpr size_t tries_arg_pos = 2;
  constexpr int seed_arg_pos = 3;

  struct MinMaxes
  {
    double min_x, max_x;
    double min_y, max_y;
  };

  struct Ellipse
  {
    double x, y, hr, vr;

    Ellipse(double x, double y, double hr, double vr);
    bool isInside(double px, double py) const noexcept;
    MinMaxes getMinMaxes() const noexcept;
  };
  size_t stringToSizeT(const char* line);
  std::pair< size_t, size_t > calc(const ellipse_list& ellipses, size_t tries, size_t seed, const MinMaxes& min_maxes);
  const std::pair< double, double > area(const ellipse_list& ellipses, size_t threads, size_t tries, size_t seed);
  MinMaxes getAllMinMaxes(const ellipse_list& ellipses);
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
      ellipses.push_back(
          {static_cast< double >(x), static_cast< double >(y), static_cast< double >(hr), static_cast< double >(hr)});
    }
    else
    {
      ellipses.push_back(
          {static_cast< double >(x), static_cast< double >(y), static_cast< double >(hr), static_cast< double >(vr)});
    }
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

bool shirokov::Ellipse::isInside(double px, double py) const noexcept
{
  const double nx = (px - x) / hr;
  const double ny = (py - y) / vr;
  return nx * nx + ny * ny <= 1.0;
}

std::pair< size_t, size_t > shirokov::calc(
    const ellipse_list& ellipses, size_t tries, size_t seed, const MinMaxes& min_maxes)
{
  std::default_random_engine engine(seed);

  std::uniform_real_distribution< double > dist_x(min_maxes.min_x, min_maxes.max_x);
  std::uniform_real_distribution< double > dist_y(min_maxes.min_y, min_maxes.max_y);

  size_t count_of_total_coverage = 0;
  size_t count_of_intersection = 0;
  for (size_t i = 0; i < tries; ++i)
  {
    const double x = dist_x(engine);
    const double y = dist_y(engine);
    bool is_in_circle = false;
    bool is_in_intersection = true;
    for (const Ellipse& s : ellipses)
    {
      if (s.isInside(x, y))
      {
        is_in_circle = true;
      }
      else
      {
        is_in_intersection = false;
      }
    }
    if (is_in_circle)
    {
      ++count_of_total_coverage;
    }
    if (is_in_intersection)
    {
      ++count_of_intersection;
    }
  }
  return {count_of_total_coverage, count_of_intersection};
}

const std::pair< double, double > shirokov::area(
    const ellipse_list& ellipses, size_t threads, size_t tries, size_t seed)
{
  if (!threads)
  {
    threads = 1;
  }

  constexpr size_t max_allowed_threads = 64;
  threads = std::min(threads, max_allowed_threads);
  threads = std::min(threads, tries);

  std::vector< std::future< std::pair< size_t, size_t > > > results;
  results.reserve(threads);

  const size_t base = tries / threads;
  const size_t remainder = tries % threads;

  const MinMaxes min_maxes = shirokov::getAllMinMaxes(ellipses);

  for (size_t i = 0; i < threads; ++i)
  {
    const size_t curr_tries_count = base + ((i < remainder) ? 1 : 0);
    results.push_back(std::async(std::launch::async, calc, std::cref(ellipses), curr_tries_count, seed + i, min_maxes));
  }

  size_t count_of_total_coverage = 0;
  size_t count_of_intersection = 0;
  for (size_t i = 0; i < threads; ++i)
  {
    const std::pair< size_t, size_t > res = results[i].get();
    count_of_total_coverage += res.first;
    count_of_intersection += res.second;
  }

  const double box_area = (min_maxes.max_x - min_maxes.min_x) * (min_maxes.max_y - min_maxes.min_y);

  const double total_coverage
      = (static_cast< double >(count_of_total_coverage) / static_cast< double >(tries)) * box_area;
  const double intersection_area
      = (static_cast< double >(count_of_intersection) / static_cast< double >(tries)) * box_area;

  return {total_coverage, intersection_area};
}

shirokov::MinMaxes shirokov::getAllMinMaxes(const ellipse_list& ellipses)
{
  MinMaxes res_mm = ellipses[0].getMinMaxes();

  for (const Ellipse& s : ellipses)
  {
    const MinMaxes curr_mm = s.getMinMaxes();
    res_mm.max_x = std::max(res_mm.max_x, curr_mm.max_x);
    res_mm.min_x = std::min(res_mm.min_x, curr_mm.min_x);
    res_mm.max_y = std::max(res_mm.max_y, curr_mm.max_y);
    res_mm.min_y = std::min(res_mm.min_y, curr_mm.min_y);
  }

  return res_mm;
}

shirokov::Ellipse::Ellipse(double x, double y, double hr, double vr):
  x(x),
  y(y),
  hr(hr),
  vr(vr)
{}

shirokov::MinMaxes shirokov::Ellipse::getMinMaxes() const noexcept
{
  return {x - hr, x + hr, y - vr, y + vr};
}
