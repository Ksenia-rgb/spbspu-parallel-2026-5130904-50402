#include "calculations.hpp"

#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "circle.hpp"
#include "polygon.hpp"

using lavrentev::area_pair_t;

lavrentev::f_t lavrentev::readInput(Polygon &pg)
{
  f_t figures;
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
    static_cast< void >(a);
    if ((x - r) < pg.getMinX())
    {
      pg.setMinX(x - r);
    }
    if ((y - r) < pg.getMinY())
    {
      pg.setMinY(y - r);
    }
    if ((x + r) > pg.getMaxX())
    {
      pg.setMaxX(x + r);
    }
    if ((y + r) > pg.getMaxY())
    {
      pg.setMaxY(y + r);
    }
    figures.push_back(Circle(r, x, y));
  }
  if (!std::cin.eof() || figures.empty())
  {
    throw std::runtime_error("Invalid input");
  }
  return figures;
}

lavrentev::count_pair_t lavrentev::calculate(const f_t &figures, const Polygon &pg, std::size_t tries, int seed)
{
  std::default_random_engine engine(seed);
  std::uniform_real_distribution< double > dist_x(pg.getMinX(), pg.getMaxX());
  std::uniform_real_distribution< double > dist_y(pg.getMinY(), pg.getMaxY());
  std::size_t res_all = 0;
  std::size_t res_is = 0;
  for (std::size_t i = 0; i < tries; ++i)
  {
    const double x = dist_x(engine);
    const double y = dist_y(engine);
    const std::size_t count_fig = countInside(figures, x, y);
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

std::size_t lavrentev::countInside(const f_t &figures, double x, double y)
{
  std::size_t res = 0;
  for (const auto &circle : figures)
  {
    const double dx = x - circle.getX();
    const double dy = y - circle.getY();
    const double radius = circle.getRadius();
    if (((dx * dx) + (dy * dy)) <= (radius * radius))
    {
      ++res;
    }
  }
  return res;
}

area_pair_t lavrentev::area(const f_t &figures, const Polygon &pg, std::size_t threads, std::size_t tries, int seed)
{
  std::vector< std::future< count_pair_t > > results;
  results.reserve(threads);
  const std::size_t base_tries = tries / threads;
  const std::size_t remainder = tries % threads;
  for (std::size_t i = 0; i < threads; ++i)
  {
    const std::size_t thread_tries = base_tries + (i == 0 ? remainder : 0);
    const int thread_seed = seed + static_cast< int >(i);
    const auto policy = std::launch::async;
    auto task = std::async(policy, calculate, std::cref(figures), std::cref(pg), thread_tries, thread_seed);
    results.push_back(std::move(task));
  }
  std::size_t total_all = 0;
  std::size_t total_is = 0;
  for (std::size_t i = 0; i < threads; ++i)
  {
    const count_pair_t res = results[i].get();
    total_all += res.first;
    total_is += res.second;
  }
  const double pg_width = static_cast< double >(pg.getMaxX() - pg.getMinX());
  const double pg_height = static_cast< double >(pg.getMaxY() - pg.getMinY());
  const double total_area = pg_width * pg_height;
  const double all_area = (total_area * static_cast< double >(total_all)) / static_cast< double >(tries);
  const double is_area = (total_area * static_cast< double >(total_is)) / static_cast< double >(tries);
  return {all_area, is_area};
}
