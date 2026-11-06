#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <functional>
#include <utility>
#include <vector>

namespace karpenkov
{
  constexpr size_t count_of_args = 4;
  constexpr size_t count_of_args_without_seed = 3;
  constexpr size_t invalid_args_count_error = 1;
  constexpr size_t base_of_ten = 10;
  constexpr size_t invalid_args = 2;
  constexpr size_t invalid_input = 3;
  struct Circle
  {
  public:
    Circle(int x, int y, int radius):
      x_(x),
      y_(y),
      radius_(radius)
    {
      if (radius <= 0)
      {
        throw std::invalid_argument("Radius must be positive");
      }
    }
    int getRadius() const
    {
      return radius_;
    }
    double left() const
    {
      return static_cast< double >(x_) - radius_;
    }
    double right() const
    {
      return static_cast< double >(x_) + radius_;
    }
    double bottom() const
    {
      return static_cast< double >(y_) - radius_;
    }
    double top() const
    {
      return static_cast< double >(y_) + radius_;
    }
    bool contains(double px, double py) const
    {
      const double dx = px - x_;
      const double dy = py - y_;
      const double r = radius_;
      return dx * dx + dy * dy <= r * r;
    }
  private:
    int x_;
    int y_;
    int radius_;
  };
  size_t stringToSizeT(const char *lms)
  {
    if (*lms == '-')
    {
      throw std::invalid_argument("Negative argument is not allowed");
    }
    char *end = nullptr;
    errno = 0;
    const unsigned long long rms = std::strtoull(lms, &end, base_of_ten);
    if (*end != '\0' || errno == ERANGE)
    {
      throw std::invalid_argument("Bad number format");
    }
    return static_cast< size_t >(rms);
  }
  struct Box
  {
    double min_x, max_x, min_y, max_y;
    double area() const
    {
      return (max_x - min_x) * (max_y - min_y);
    }
  };
  Box boundingBox(const std::vector< Circle > &circles)
  {
    Box box{circles[0].left(), circles[0].right(), circles[0].bottom(), circles[0].top()};
    for (const Circle &c : circles)
    {
      box.min_x = std::min(box.min_x, c.left());
      box.max_x = std::max(box.max_x, c.right());
      box.min_y = std::min(box.min_y, c.bottom());
      box.max_y = std::max(box.max_y, c.top());
    }
    return box;
  }
  std::pair< size_t, size_t > monteCarloGenerator(
      const std::vector< Circle > &circles, const Box &box, size_t count, size_t seed)
  {
    std::default_random_engine random_generator(seed);
    std::uniform_real_distribution< double > dist_x(box.min_x, box.max_x);
    std::uniform_real_distribution< double > dist_y(box.min_y, box.max_y);

    size_t in_union = 0;
    size_t in_intersection = 0;
    for (size_t i = 0; i < count; ++i)
    {
      const double x = dist_x(random_generator);
      const double y = dist_y(random_generator);

      size_t inside = 0;
      for (const Circle &c : circles)
      {
        if (c.contains(x, y))
        {
          ++inside;
        }
      }
      if (inside > 0)
      {
        ++in_union;
      }
      if (inside == circles.size())
      {
        ++in_intersection;
      }
    }
    return {in_union, in_intersection};
  }
  struct Areas
  {
    double union_area;
    double intersection_area;
  };
  Areas getAreas(const std::vector< Circle > &circles, size_t threads, size_t tries, size_t seed)
  {
    const Box box = boundingBox(circles);
    const size_t base = tries / threads;
    const size_t rem = tries % threads;

    std::vector< std::future< std::pair< size_t, size_t > > > results;
    for (size_t i = 0; i < threads; ++i)
    {
      const size_t count = base + (i < rem ? 1 : 0);
      results.push_back(
          std::async(std::launch::async, monteCarloGenerator, std::cref(circles), std::cref(box), count, seed + i));
    }

    size_t in_union = 0;
    size_t in_intersection = 0;
    for (auto &f : results)
    {
      const std::pair< size_t, size_t > part = f.get();
      in_union += part.first;
      in_intersection += part.second;
    }

    const double box_area = box.area();
    const double total = static_cast< double >(tries);
    return {
        box_area * static_cast< double >(in_union) / total, box_area * static_cast< double >(in_intersection) / total};
  }
  bool readCircles(std::istream &in, std::vector< Circle > &out)
  {
    int radius = 0, x = 0, y = 0;
    std::string second;
    while (in >> radius)
    {
      if (!(in >> second >> x >> y) || radius <= 0)
      {
        return false;
      }
      out.emplace_back(x, y, radius);
    }
    return in.eof();
  }
}

int main(int argc, const char *argv[])
{
  if (argc != karpenkov::count_of_args_without_seed && argc != karpenkov::count_of_args)
  {
    std::cerr << "Invalid count of arguments\n";
    return karpenkov::invalid_args_count_error;
  }
  size_t threads = 0, tries = 0, seed = 0;
  try
  {
    threads = karpenkov::stringToSizeT(argv[1]);
    tries = karpenkov::stringToSizeT(argv[2]);
    if (argc == karpenkov::count_of_args)
    {
      seed = karpenkov::stringToSizeT(argv[3]);
    }
  }
  catch (const std::invalid_argument &e)
  {
    std::cerr << "Invalid argument: " << e.what() << '\n';
    return karpenkov::invalid_args;
  }
  if (tries == 0)
  {
    std::cerr << "Tries must be postivie number";
    return karpenkov::invalid_args;
  }
  if (threads == 0)
  {
    threads = 1;
  }
  std::vector< karpenkov::Circle > circles;
  if (!karpenkov::readCircles(std::cin, circles))
  {
    std::cerr << "Invalid input\n";
    return karpenkov::invalid_input;
  }
  karpenkov::Areas areas{0.0, 0.0};
  if (!circles.empty())
  {
    areas = karpenkov::getAreas(circles, threads, tries, seed);
  }
  std::cout << std::setprecision(std::numeric_limits< double >::max_digits10) << areas.union_area << ' '
            << areas.intersection_area << '\n';
  return 0;
}
