#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace saldaev
{
  struct Point
  {
    double x, y;
  };

  struct Circle
  {
    double radius;
    Point position;
    bool contain(Point point) const noexcept;
  };

  struct Frame
  {
    Point min, max;
  };

  std::pair< double, double > area(const std::vector< Circle > &circles, size_t threads, size_t tries, size_t seed);
  std::pair< size_t, size_t > calculate(Frame frame, const std::vector< Circle > &circles, size_t tries, size_t seed);
  Frame getBorders(const std::vector< Circle > &circles);
};

int main(int argc, char **argv)
{
  if (argc != 3 && argc != 4) {
    std::cerr << "wrong ammount of arguments";
    return 1;
  }
  if (argv[1][0] == '-' || argv[2][0] == '-') {
    std::cerr << "Negative argument\n";
    return 1;
  }

  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;
  try {
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    threads = threads ? threads : 1;
    if (tries == 0) {
      std::cerr << "tries must be positive number\n";
      return 1;
    }
    if (argc == 4) {
      if (argv[3][0] == '-') {
        std::cerr << "Negative argument\n";
        return 1;
      }
      seed = std::stoull(argv[3]);
    }
  } catch (const std::invalid_argument &) {
    std::cerr << "Not a number in arguments\n";
    return 1;
  } catch (const std::out_of_range &) {
    std::cerr << "Overflow in arguments\n";
    return 1;
  }

  double r = 0, place_holder = 0, x = 0, y = 0;
  std::vector< saldaev::Circle > circles;
  while (std::cin >> r >> place_holder >> x >> y) {
    circles.push_back({r, {x, y}});
  }
  if (!std::cin.eof()) {
    std::cerr << "invalid input";
    return 1;
  }

  if (circles.size() == 0) {
    std::cout << "0.0 0.0\n";
    return 0;
  }

  const std::pair< double, double > areas = saldaev::area(circles, threads, tries, seed);

  std::cout << areas.first << ' ' << areas.second << '\n';
}

std::pair< double, double > saldaev::area(const std::vector< Circle > &circles, size_t threads, size_t tries,
                                          size_t seed)
{
  std::vector< std::future< std::pair< size_t, size_t > > > futures;

  const size_t tries_per_thread = tries / threads;
  const size_t remainder = tries % threads;

  Frame frame = getBorders(circles);
  for (size_t i = 0; i < threads; ++i) {
    size_t part = tries_per_thread + (i < remainder ? 1 : 0);
    futures.push_back(std::async(std::launch::async, calculate, frame, std::cref(circles), part, seed + i));
  }

  size_t hits = 0;
  size_t complete_hits = 0;
  for (size_t i = 0; i < threads; i++) {
    const std::pair< size_t, size_t > res = futures[i].get();
    hits += res.first;
    complete_hits += res.second;
  }

  const double frame_area = (frame.max.x - frame.min.x) * (frame.max.y - frame.min.y);
  return {frame_area * (static_cast< double >(hits) / tries),
          frame_area * (static_cast< double >(complete_hits) / tries)};
}

std::pair< size_t, size_t > saldaev::calculate(Frame frame, const std::vector< Circle > &circles, size_t tries,
                                               size_t seed)
{
  std::default_random_engine gen(seed);
  std::uniform_real_distribution< double > width(frame.min.x, frame.max.x);
  std::uniform_real_distribution< double > hight(frame.min.y, frame.max.y);

  size_t hits = 0;
  size_t complete_hits = 0;

  for (size_t i = 0; i < tries; ++i) {
    const Point p{width(gen), hight(gen)};
    bool in_all = true;
    bool in_any = false;

    for (size_t j = 0; j < circles.size(); ++j) {
      if (circles[j].contain(p)) {
        in_any = true;
      } else {
        in_all = false;
      }
    }

    if (in_any) {
      hits++;
      if (in_all) {
        complete_hits++;
      }
    }
  }

  return {hits, complete_hits};
}

saldaev::Frame saldaev::getBorders(const std::vector< Circle > &circles)
{
  const double lim = std::numeric_limits< double >::infinity();

  Point min{lim, lim};
  Point max{-lim, -lim};

  for (size_t i = 0; i < circles.size(); ++i) {
    const Circle cir = circles[i];
    const Point pos = cir.position;
    const double r = cir.radius;
    min = {min.x > pos.x - r ? pos.x - r : min.x, min.y > pos.y - r ? pos.y - r : min.y};
    max = {max.x < pos.x + r ? pos.x + r : max.x, max.y < pos.y + r ? pos.y + r : max.y};
  }

  return {min, max};
}