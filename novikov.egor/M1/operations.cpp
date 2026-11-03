#include "operations.hpp"

#include "circle.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <future>
#include <limits>
#include <random>
#include <utility>
#include <vector>

novikov::MinMaxes::MinMaxes():
  min_x_(std::numeric_limits< double >::max()),
  max_x_(std::numeric_limits< double >::lowest()),
  min_y_(std::numeric_limits< double >::max()),
  max_y_(std::numeric_limits< double >::lowest())
{}

double novikov::MinMaxes::getMinX() const
{
  return min_x_;
}
double novikov::MinMaxes::getMinY() const
{
  return min_y_;
}
double novikov::MinMaxes::getMaxX() const
{
  return max_x_;
}
double novikov::MinMaxes::getMaxY() const
{
  return max_y_;
}

void novikov::MinMaxes::setMinX(double v)
{
  min_x_ = v;
}
void novikov::MinMaxes::setMinY(double v)
{
  min_y_ = v;
}
void novikov::MinMaxes::setMaxX(double v)
{
  max_x_ = v;
}
void novikov::MinMaxes::setMaxY(double v)
{
  max_y_ = v;
}

std::size_t novikov::countInside(const std::vector< Circle >& circles, double x, double y)
{
  std::size_t res = 0;
  for (const Circle& c : circles)
  {
    const double dx = x - c.getX();
    const double dy = y - c.getY();
    const double r = c.getR();
    if (dx * dx + dy * dy <= r * r)
    {
      ++res;
    }
  }
  return res;
}

novikov::MinMaxes novikov::getMinMaxes(const std::vector< Circle >& circles)
{
  MinMaxes mm;
  mm.setMinX(circles[0].getX() - circles[0].getR());
  mm.setMaxX(circles[0].getX() + circles[0].getR());
  mm.setMinY(circles[0].getY() - circles[0].getR());
  mm.setMaxY(circles[0].getY() + circles[0].getR());

  for (const Circle& c : circles)
  {
    mm.setMinX(std::min(mm.getMinX(), c.getX() - c.getR()));
    mm.setMaxX(std::max(mm.getMaxX(), c.getX() + c.getR()));
    mm.setMinY(std::min(mm.getMinY(), c.getY() - c.getR()));
    mm.setMaxY(std::max(mm.getMaxY(), c.getY() + c.getR()));
  }

  return mm;
}

std::pair< std::size_t, std::size_t > novikov::calc(
    const std::vector< Circle >& circles, std::size_t tries, std::size_t seed, const MinMaxes& min_maxes)
{
  std::default_random_engine engine(seed);
  std::uniform_real_distribution< double > dist_x(min_maxes.getMinX(), min_maxes.getMaxX());
  std::uniform_real_distribution< double > dist_y(min_maxes.getMinY(), min_maxes.getMaxY());

  std::size_t count_of_total_coverage = 0;
  std::size_t count_of_intersection = 0;

  for (std::size_t i = 0; i < tries; ++i)
  {
    const double px = dist_x(engine);
    const double py = dist_y(engine);

    const std::size_t count = countInside(circles, px, py);

    if (count > 0)
    {
      ++count_of_total_coverage;
    }
    if (count == circles.size())
    {
      ++count_of_intersection;
    }
  }

  return {count_of_total_coverage, count_of_intersection};
}

std::pair< double, double > novikov::area(
    const std::vector< Circle >& circles, std::size_t threads, std::size_t tries, std::size_t seed)
{
  if (threads > tries)
  {
    threads = tries;
  }

  const MinMaxes min_maxes = novikov::getMinMaxes(circles);

  const std::size_t base = tries / threads;
  const std::size_t remainder = tries % threads;

  std::vector< std::future< std::pair< std::size_t, std::size_t > > > results;
  results.reserve(threads);

  for (std::size_t i = 0; i < threads; ++i)
  {
    const std::size_t curr_tries = base + ((i < remainder) ? 1 : 0);
    results.push_back(
        std::async(std::launch::async, novikov::calc, std::cref(circles), curr_tries, seed + i, std::cref(min_maxes)));
  }

  std::size_t count_of_total_coverage = 0;
  std::size_t count_of_intersection = 0;

  for (std::size_t i = 0; i < threads; ++i)
  {
    const std::pair< std::size_t, std::size_t > res = results[i].get();
    count_of_total_coverage += res.first;
    count_of_intersection += res.second;
  }

  const double box_area = (min_maxes.getMaxX() - min_maxes.getMinX()) * (min_maxes.getMaxY() - min_maxes.getMinY());

  const double total_coverage
      = (static_cast< double >(count_of_total_coverage) / static_cast< double >(tries)) * box_area;
  const double intersection_area
      = (static_cast< double >(count_of_intersection) / static_cast< double >(tries)) * box_area;

  return {total_coverage, intersection_area};
}
