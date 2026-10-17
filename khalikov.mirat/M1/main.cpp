#include <iostream>
#include <string>
#include <cstddef>
#include <vector>
#include <sstream>
#include "Point.hpp"

namespace khalikov {
  struct Circle
  {
    khalikov::Point center;
    double r;
  };
}

int main(int argc, char** argv)
{
  if (argc > 4 || argc < 3)
  {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }

  int threads = std::stoi(argv[1]);
  size_t tries = std::stoull(argv[2]);

  if (threads <= 0 || tries == 0)
  {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }

  size_t seed = 0;

  if (argc == 4)
  {
    seed = std::stoull(argv[3]);
  }

  (void)seed;

  std::vector< khalikov::Circle > circles;
  std::string line;

  while (std::getline(std::cin, line))
  {
    std::istringstream iss(line);
    int r, c, x, y;
    std::string extra;

    if (!(iss >> r >> c >> x >> y) || (iss >> extra) || (r <= 0))
    {
      std::cerr << "Incorrect input\n";
      return 1;
    }

    khalikov::Circle circle;
    circle.center.x = x;
    circle.center.y = y;
    circle.r = r;
    circles.push_back(circle);
  }

  if (std::cin.fail() && !std::cin.eof())
  {
    std::cerr << "Incorrect input\n";
    return 1;
  }
}
