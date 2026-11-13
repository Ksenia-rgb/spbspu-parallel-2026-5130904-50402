#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include "shapes.hpp"

bool getCircle(std::string& line, vishnevskiy::Circle& c)
{
  std::stringstream ss(line);
  long long r = 0;
  long long ignore = 0;
  long long x = 0;
  long long y = 0;
  if (!(ss >> r >> ignore >> x >> y))
  {
    return false;
  }
  if (r <= 0)
  {
    return false;
  }
  c.r = r;
  c.x = x;
  c.y = y;
  return true;
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    std::cerr << "Wrong arg amount!\n";
    return 1;
  }
  size_t threads = 0;
  size_t tests = 0;
  size_t seed = 0;
  try
  {
    threads = std::stoull(argv[1]);
    tests = std::stoull(argv[2]);
    if (argc == 4)
    {
      seed = std::stoull(argv[3]);
    }
    else
    {
      seed = 0;
    }
  }
  catch (...)
  {
    std::cerr << "Bad args!\n";
    return 1;
  }
  if (threads == 0)
  {
    std::cerr << "Wrong thread number!\n";
    return 1;
  }
  if (tests == 0)
  {
    std::cerr << "Not enough tests!\n";
    return 1;
  }
  std::string line;
  std::vector<vishnevskiy::Circle> circles;
  while (std::getline(std::cin, line))
  {
    vishnevskiy::Circle c;
    if (!getCircle(line, c))
    {
      std::cerr << "Invalid shape!\n";
      return 1;
    }
    circles.push_back(c);
  }
  if (circles.empty())
  {
    return 0;
  }
  vishnevskiy::CircleBoundaries b = getBoundaries(circles);
  size_t resUnion = vishnevskiy::runUnion(circles, threads, tests, seed, b);
  size_t resIntersection = vishnevskiy::runIntersection(circles, threads, tests, seed, b);
  double area = static_cast<double>(b.area());
  double unionArea = (static_cast<double>(resUnion) / static_cast<double>(tests)) * area;
  double intersectionArea = (static_cast<double>(resIntersection) / static_cast<double>(tests)) * area;
  std::cout << unionArea << " " << intersectionArea << "\n";
  return 0;
}
