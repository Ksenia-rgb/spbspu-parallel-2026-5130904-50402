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
  long long threads = 0;
  long long tests = 0;
  long long seed = 0;
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
  if (threads < 0)
  {
    std::cerr << "Wrong thread number!\n";
    return 1;
  }
  if (tests < 0)
  {
    std::cerr << "Wrong test number!\n";
    return 1;
  }
  if (seed < 0)
  {
    std::cerr << "Wrong seed number!\n";
    return 1;
  }
  if (tests == 0)
  {
    std::cerr << "Not enough tests!\n";
    return 1;
  }
  std::string line;
  std::vector< vishnevskiy::Circle > circles;
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
  const double area = static_cast< double >(b.area());
  if (threads == 0)
  {
    const double r_u =
      (static_cast< double >(calcUnion(circles, tests, seed, b)) / static_cast< double >(tests));
    std::cout << r_u * area << " ";
    const double r_i =
      (static_cast< double >(calcIntersection(circles, tests, seed, b)) / static_cast< double >(tests));
    std::cout << r_i * area;
    std::cout << "\n";
    return 0;
  }
  const size_t res_union = vishnevskiy::runUnion(circles, threads, tests, seed, b);
  const size_t res_intersection = vishnevskiy::runIntersection(circles, threads, tests, seed, b);
  const double union_area = (static_cast< double >(res_union) / static_cast< double >(tests)) * area;
  const double intersection_area =
    (static_cast< double >(res_intersection) / static_cast< double >(tests)) * area;
  std::cout << union_area << " " << intersection_area << "\n";
  return 0;
}
