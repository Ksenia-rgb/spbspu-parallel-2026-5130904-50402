#include <algorithm>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "Circle.hpp"
#include "Ellipse.hpp"
#include "Frame.hpp"
#include "Point.hpp"
#include "Shapes.hpp"

namespace khalikov
{
  using shapes_t = std::vector< std::unique_ptr< khalikov::Shapes > >;

  khalikov::Frame findFrames(const shapes_t &shapes)
  {
    auto it = shapes.begin();
    khalikov::Frame frame;
    frame.min_x = (*it)->minX();
    frame.max_x = (*it)->maxX();
    frame.min_y = (*it)->minY();
    frame.max_y = (*it)->maxY();
    it++;

    for (; it != shapes.end(); it++) {
      frame.min_x = std::min(frame.min_x, (*it)->minX());
      frame.max_x = std::max(frame.max_x, (*it)->maxX());
      frame.min_y = std::min(frame.min_y, (*it)->minY());
      frame.max_y = std::max(frame.max_y, (*it)->maxY());
    }
    return frame;
  }

  Point genPoint(const khalikov::Frame &frame, std::default_random_engine &gen)
  {
    std::uniform_real_distribution< double > dist_x(frame.min_x, frame.max_x);
    std::uniform_real_distribution< double > dist_y(frame.min_y, frame.max_y);
    Point pt;
    pt.x = dist_x(gen);
    pt.y = dist_y(gen);
    return pt;
  }

  bool isInUnion(const shapes_t &shapes, const Point &pt)
  {
    for (auto it = shapes.begin(); it != shapes.end(); it++) {
      if ((*it)->isIn(pt)) {
        return true;
      }
    }
    return false;
  }

  bool isInIntersection(const shapes_t &shapes, const Point &pt)
  {
    for (auto it = shapes.begin(); it != shapes.end(); it++) {
      if (!(*it)->isIn(pt)) {
        return false;
      }
    }
    return true;
  }

  double getFrameArea(const khalikov::Frame &frame)
  {
    return (frame.max_x - frame.min_x) * (frame.max_y - frame.min_y);
  }

  double getUnionArea(size_t tries, size_t union_hits, double frame_area)
  {
    return frame_area * union_hits / tries;
  }

  double getIntersectionArea(size_t tries, size_t intersection_hits, double frame_area)
  {
    return frame_area * intersection_hits / tries;
  }

  std::pair< size_t, size_t > calculate(const shapes_t &shapes, const khalikov::Frame &frame, size_t tries, size_t seed)
  {
    std::default_random_engine gen(seed);
    size_t union_hits = 0;
    size_t intersection_hits = 0;
    for (size_t i = 0; i < tries; i++) {
      const Point example = genPoint(frame, gen);
      intersection_hits += isInIntersection(shapes, example);
      union_hits += isInUnion(shapes, example);
    }
    return {union_hits, intersection_hits};
  }
}

int main(int argc, char **argv)
{
  if (argc > 4 || argc < 3) {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }

  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      std::cerr << "Incorrect arguments\n";
      return 1;
    }
  }

  try {
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    if (argc == 4) {
      seed = std::stoull(argv[3]);
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << "Incorrect arguments\n";
    return 2;
  }

  if (tries == 0) {
    std::cerr << "Incorrect arguments\n";
    return 2;
  }

  threads = (threads == 0) ? 1 : threads;

  khalikov::shapes_t shapes;
  std::string line;

  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    int r = 0;
    int c = 0;
    int x = 0;
    int y = 0;
    std::string extra;

    if (!(iss >> r >> c >> x >> y) || (iss >> extra)) {
      std::cerr << "Incorrect input\n";
      return 1;
    }

    if (c == 0) {
      auto circle = std::make_unique< khalikov::Circle >();
      circle->center.x = x;
      circle->center.y = y;
      circle->r = r;
      shapes.push_back(std::move(circle));
    }

    else {
      auto ellipse = std::make_unique< khalikov::Ellipse >();
      ellipse->center.x = x;
      ellipse->center.y = y;
      ellipse->a = r;
      ellipse->b = c;
      shapes.push_back(std::move(ellipse));
    }
  }

  if (std::cin.fail() && !std::cin.eof()) {
    std::cerr << "Incorrect input\n";
    return 1;
  }

  if (shapes.empty()) {
    std::cout << "0 0\n";
    return 0;
  }

  const khalikov::Frame frame = khalikov::findFrames(shapes);
  const size_t base_tries = tries / threads;
  const size_t remainder = tries % threads;
  std::vector< std::future< std::pair< size_t, size_t > > > futures;

  for (size_t i = 0; i < threads; i++) {
    const size_t local_tries = base_tries + (i < remainder ? 1 : 0);
    const size_t local_seed = seed + i;
    futures.push_back(std::async(std::launch::async, khalikov::calculate, std::cref(shapes), std::cref(frame),
                                 local_tries, local_seed));
  }
  size_t union_hits = 0;
  size_t intersection_hits = 0;
  for (size_t i = 0; i < futures.size(); i++) {
    auto result = futures[i].get();
    union_hits += result.first;
    intersection_hits += result.second;
  }
  const double frame_area = khalikov::getFrameArea(frame);
  std::cout << khalikov::getUnionArea(tries, union_hits, frame_area) << ' ';
  std::cout << khalikov::getIntersectionArea(tries, intersection_hits, frame_area) << '\n';
}
