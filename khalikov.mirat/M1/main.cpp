#include <iostream>
#include <string>
#include <cstddef>
#include <vector>
#include <sstream>
#include <algorithm>
#include <random>
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

  Point genPoint(const Frame& frame, std::mt19937& gen)
  {
    std::uniform_real_distribution< double > dist_x(frame.min_x, frame.max_x);
    std::uniform_real_distribution< double > dist_y(frame.min_y, frame.max_y);
    Point pt;
    pt.x = dist_x(gen);
    pt.y = dist_y(gen);
    return pt;
  }

  bool isInUnion(const std::vector< Circle >& circles, const Point& pt)
  {
    for (auto it = circles.begin(); it != circles.end(); it++)
    {
      if (isIn(*it, pt))
      {
        return true;
      }
    }
    return false;
  }

  bool isInIntersection(const std::vector< Circle >& circles, const Point& pt)
  {
    for (auto it = circles.begin(); it != circles.end(); it++)
    {
      if (!isIn(*it, pt))
      {
        return false;
      }
    }
    return true;
  }

  double getFrameArea(const Frame& frame)
  {
    return ((frame.max_x - frame.min_x) * (frame.max_y - frame.min_y));
  }

  double getUnionArea(size_t tries, size_t union_hits, double frame_area)
  {
    return frame_area * union_hits / tries;
  }

  double getIntersectionArea(size_t tries, size_t intersection_hits, double frame_area)
  {
    return frame_area * intersection_hits / tries;
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

  khalikov::Frame frame = khalikov::findFrames(circles);
  std::mt19937 gen(seed);
  size_t union_hits = 0;
  size_t intersection_hits = 0;

  for (size_t i = 0; i < tries; i++)
  {
    khalikov::Point example = genPoint(frame, gen);
    intersection_hits += isInIntersection(circles, example);
    union_hits += isInUnion(circles, example);
  }

  double frame_area = khalikov::getFrameArea(frame);
  std::cout << khalikov::getUnionArea(tries, union_hits, frame_area) << ' ';
  std::cout << khalikov::getIntersectionArea(tries, intersection_hits, frame_area) << '\n';
}
