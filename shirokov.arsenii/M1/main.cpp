#include <cctype>
#include <cerrno>
#include <future>
#include <iostream>
#include <limits>
#include <new>
#include <random>
#include <stdexcept>
#include <vector>

namespace shirokov
{
  struct Circle
  {
    double r, x, y;
  };

  struct MinMaxes
  {
    double minX, maxX;
    double minY, maxY;
  };

  size_t stringToSizeT(const char* line);
  bool isInside(double x, double y, const Circle& circle);
  std::pair< size_t, size_t > calc(
      const std::vector< Circle >& circles, size_t tries, size_t seed, const MinMaxes& minMaxes);
  std::pair< double, double > area(const std::vector< Circle >& circles, size_t threads, size_t tries, size_t seed);
  MinMaxes getMinMaxes(const std::vector< Circle >& circles);
}

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Not enough arguments" << '\n';
    return 1;
  }

  size_t threads = 0, tries = 0, seed = 0;
  try
  {
    threads = shirokov::stringToSizeT(argv[1]);
    tries = shirokov::stringToSizeT(argv[2]);
    if (!tries)
    {
      std::cerr << "number of tries is not positive" << '\n';
      return 1;
    }
    if (argc == 4)
    {
      seed = shirokov::stringToSizeT(argv[3]);
    }
  }
  catch (const std::invalid_argument& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }

  std::vector< shirokov::Circle > circles;
  int _;
  long long r = 0, x = 0, y = 0;
  while (std::cin >> r >> _ >> x >> y)
  {
    if (r < 0)
    {
      std::cerr << "Negative radius" << '\n';
      return 2;
    }
    circles.push_back({static_cast< double >(r), static_cast< double >(x), static_cast< double >(y)});
  }
  if (circles.empty())
  {
    std::cerr << "Input error" << '\n';
    return 2;
  }

  std::pair< double, double > res = shirokov::area(circles, threads, tries, seed);
  std::cout << res.first << ' ' << res.second << '\n';
  return 0;
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
  unsigned long long res = std::strtoull(str, &end, 10);
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

bool shirokov::isInside(double x, double y, const Circle& circle)
{
  double dx = x - circle.x;
  double dy = y - circle.y;
  return dx * dx + dy * dy <= circle.r * circle.r;
}

std::pair< size_t, size_t > shirokov::calc(
    const std::vector< Circle >& circles, size_t tries, size_t seed, const MinMaxes& minMaxes)
{
  std::default_random_engine engine(seed);

  std::uniform_real_distribution< double > distX(minMaxes.minX, minMaxes.maxX);
  std::uniform_real_distribution< double > distY(minMaxes.minY, minMaxes.maxY);

  size_t countOfTotalCoverage = 0;
  size_t countOfIntersection = 0;
  for (size_t i = 0; i < tries; ++i)
  {
    double x = distX(engine);
    double y = distY(engine);
    bool isInCircle = false;
    bool isInIntersection = true;
    for (const Circle& c : circles)
    {
      if (isInside(x, y, c))
      {
        isInCircle = true;
      }
      else
      {
        isInIntersection = false;
      }
    }
    if (isInCircle)
    {
      ++countOfTotalCoverage;
    }
    if (isInIntersection)
    {
      ++countOfIntersection;
    }
  }
  return {countOfTotalCoverage, countOfIntersection};
}

std::pair< double, double > shirokov::area(
    const std::vector< Circle >& circles, size_t threads, size_t tries, size_t seed)
{
  if (!threads)
  {
    threads = 1;
  }

  std::vector< std::future< std::pair< size_t, size_t > > > results;
  try
  {
    results.reserve(threads);
  }
  catch (const std::bad_alloc&)
  {
    threads = 1;
  }
  size_t base = tries / threads;
  size_t remainder = tries % threads;

  MinMaxes minMaxes = shirokov::getMinMaxes(circles);

  for (size_t i = 0; i < threads; ++i)
  {
    size_t currTriesCount = base + ((i < remainder) ? 1 : 0);
    results.push_back(std::async(std::launch::async, calc, circles, currTriesCount, seed + i, minMaxes));
  }

  size_t countOfTotalCoverage = 0;
  size_t countOfIntersection = 0;
  for (size_t i = 0; i < threads; ++i)
  {
    std::pair< size_t, size_t > res = results[i].get();
    countOfTotalCoverage += res.first;
    countOfIntersection += res.second;
  }

  double boxArea = (minMaxes.maxX - minMaxes.minX) * (minMaxes.maxY - minMaxes.minY);

  double totalCoverage = (static_cast< double >(countOfTotalCoverage) / static_cast< double >(tries)) * boxArea;
  double intersectionArea = (static_cast< double >(countOfIntersection) / static_cast< double >(tries)) * boxArea;

  return {totalCoverage, intersectionArea};
}

shirokov::MinMaxes shirokov::getMinMaxes(const std::vector< Circle >& circles)
{
  double minX = circles[0].x - circles[0].r;
  double maxX = circles[0].x + circles[0].r;
  double minY = circles[0].y - circles[0].r;
  double maxY = circles[0].y + circles[0].r;

  for (const auto& c : circles)
  {
    minX = std::min(minX, c.x - c.r);
    maxX = std::max(maxX, c.x + c.r);
    minY = std::min(minY, c.y - c.r);
    maxY = std::max(maxY, c.y + c.r);
  }

  return {minX, maxX, minY, maxY};
}
