#include <iostream>
#include <string>
#include <cstddef>
#include <vector>
#include <sstream>
#include <algorithm>
#include "Point.hpp"

namespace khalikov {
  struct Circle
  {
    khalikov::Point center;
    double r;
  };

  struct Frame
  {
    double min_x;
    double max_x;
    double min_y;
    double max_y;
  };

  bool isIn(const Circle& circle, const Point& pt)
  {
    double dx = pt.x - circle.center.x;
    double dy = pt.y - circle.center.y;
    return dx * dx + dy * dy <= circle.r * circle.r;
  }

  Frame findFrames(const std::vector< Circle >& circles)
  {
    auto it = circles.begin();
    Frame frame;
    frame.min_x = it->center.x - it->r;
    frame.max_x = it->center.x + it->r;
    frame.min_y = it->center.y - it->r;
    frame.max_y = it->center.y + it->r;
    it++;

    for (; it != circles.end(); it++)
    {
      frame.min_x = std::min(frame.min_x, it->center.x - it->r);
      frame.max_x = std::max(frame.max_x, it->center.x + it->r);
      frame.min_y = std::min(frame.min_y, it->center.y - it->r);
      frame.max_y = std::max(frame.max_y, it->center.y + it->r);
    }
    return frame;
  }

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
  (void)seed;


  if (argc == 4)
  {
    seed = std::stoull(argv[3]);
  }

  std::vector< khalikov::Circle > circles;
  std::string line;

  while (std::getline(std::cin, line))
  {
    std::istringstream iss(line);
    int r, c, x, y;
    std::string extra;

    if (!(iss >> r >> c >> x >> y) || (iss >> extra))
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
