#include "calculations.hpp"

#include <iostream>
#include <vector>
#include <future>
#include <random>
#include <utility>
#include <cstddef>
#include <stdexcept>
#include <functional>

std::vector< lavrentev::Circle > lavrentev::readInput(lavrentev::Polygon &pg)
{
  std::vector< lavrentev::Circle > figures;
  int r = 0;
  while (std::cin >> r)
  {
    int a = 0;
    int x = 0;
    int y = 0;
    if (!(std::cin >> a >> x >> y))
    {
      throw std::runtime_error("Invalid figure parameters");
    }

    if (x - r < pg.getMinX())
    {
      pg.setMinX(x - r);
    }
    if (y - r < pg.getMinY())
    {
      pg.setMinY(y - r);
    }
    if (x + r > pg.getMaxX())
    {
      pg.setMaxX(x + r);
    }
    if (y + r > pg.getMaxY())
    {
      pg.setMaxY(y + r);
    }

    figures.push_back(lavrentev::Circle(r, x, y));
  }
  if (!std::cin.eof() || figures.empty())
  {
    throw std::runtime_error("Invalid input");
  }
  return figures;
}

std::pair< size_t, size_t > lavrentev::calculate(const std::vector< lavrentev::Circle > &figures,
  const lavrentev::Polygon &pg,
  size_t tries,
  int seed)
{
  std::default_random_engine engine(seed);

  std::uniform_real_distribution< double > dist_x(pg.getMinX(), pg.getMaxX());
  std::uniform_real_distribution< double > dist_y(pg.getMinY(), pg.getMaxY());

  size_t res_all = 0;
  size_t res_is = 0;
  for (size_t i = 0; i < tries; ++i)
  {
    const double x = dist_x(engine);
    const double y = dist_y(engine);
    const size_t count_fig = countInside(figures, x, y);
    if (count_fig > 0)
    {
      ++res_all;
    }
    if (count_fig == figures.size())
    {
      ++res_is;
    }
  }
  return {res_all, res_is};
}

size_t lavrentev::countInside(const std::vector< lavrentev::Circle > &figures, double x, double y)
{
  size_t res = 0;
  for (const auto &circle : figures)
  {
    const double dx = x - circle.getX();
    const double dy = y - circle.getY();
    if (dx * dx + dy * dy <= circle.getRadius() * circle.getRadius())
    {
      ++res;
    }
  }
  return res;
}

std::pair< double, double > lavrentev::area(const std::vector< lavrentev::Circle > &figures,
  const lavrentev::Polygon &pg,
  size_t threads,
  size_t tries,
  int seed)
{
  std::vector< std::future< std::pair< size_t, size_t > > > results;
  results.reserve(threads);
  const size_t base_tries = tries / threads;
  const size_t remainder = tries % threads;

  for (size_t i = 0; i < threads; ++i)
  {
    const size_t thread_tries = base_tries + (i == 0 ? remainder : 0);

    results.push_back(std::async(std::launch::async,
      lavrentev::calculate,
      std::cref(figures),
      std::cref(pg),
      thread_tries,
      seed + static_cast< int >(i)
    ));
  }

  size_t total_all = 0;
  size_t total_is = 0;
  for (size_t i = 0; i < threads; ++i)
  {
    const std::pair< size_t, size_t > res = results[i].get();
    total_all += res.first;
    total_is += res.second;
  }

  const double pg_width = static_cast< double >(pg.getMaxX() - pg.getMinX());
  const double pg_height = static_cast< double >(pg.getMaxY() - pg.getMinY());
  const double total_area = pg_width * pg_height;

  const double all_area = total_area * static_cast< double >(total_all) / static_cast< double >(tries);
  const double is_area = total_area * static_cast< double >(total_is) / static_cast< double >(tries);

  return {all_area, is_area};
}
