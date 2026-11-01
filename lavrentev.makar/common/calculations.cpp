#include "calculations.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <circle.hpp>

using lavrentev::area_pair_t;

namespace
{
  struct FastCircle
  {
    double x;
    double y;
    double r2;
  };
}

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
    if (figures.empty())
    {
      pg = Polygon(x - r, y - r, x + r, y + r);
    }
    else
    {
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
  const std::size_t fig_size = figures.size();
  if (fig_size == 0 || tries == 0)
  {
    return {0, 0};
  }

  std::uint64_t s = static_cast< std::uint32_t >(seed) % 2147483647ULL;
  if (s == 0)
  {
    s = 1ULL;
  }

  const double min_x = static_cast< double >(pg.getMinX());
  const double min_y = static_cast< double >(pg.getMinY());
  const double base = 2147483646.0;
  const double denom = base * base;
  const double scale_x = static_cast< double >(pg.getMaxX() - pg.getMinX()) / denom;
  const double scale_y = static_cast< double >(pg.getMaxY() - pg.getMinY()) / denom;

  std::size_t res_all = 0;
  std::size_t res_is = 0;

  if (fig_size == 1)
  {
    const double cx = static_cast< double >(figures[0].getX());
    const double cy = static_cast< double >(figures[0].getY());
    const double r = static_cast< double >(figures[0].getRadius());
    const double cr2 = r * r;

    for (std::size_t i = 0; i < tries; ++i)
    {
      std::uint64_t p1 = s * 16807ULL;
      s = (p1 & 0x7FFFFFFFULL) + (p1 >> 31);
      if (s >= 2147483647ULL) s -= 2147483647ULL;
      const double r1 = static_cast< double >(s - 1ULL);

      std::uint64_t p2 = s * 16807ULL;
      s = (p2 & 0x7FFFFFFFULL) + (p2 >> 31);
      if (s >= 2147483647ULL) s -= 2147483647ULL;
      const double r2 = static_cast< double >(s - 1ULL);

      const double x = min_x + (r1 + r2 * base) * scale_x;

      std::uint64_t p3 = s * 16807ULL;
      s = (p3 & 0x7FFFFFFFULL) + (p3 >> 31);
      if (s >= 2147483647ULL) s -= 2147483647ULL;
      const double r3 = static_cast< double >(s - 1ULL);

      std::uint64_t p4 = s * 16807ULL;
      s = (p4 & 0x7FFFFFFFULL) + (p4 >> 31);
      if (s >= 2147483647ULL) s -= 2147483647ULL;
      const double r4 = static_cast< double >(s - 1ULL);

      const double y = min_y + (r3 + r4 * base) * scale_y;

      const double dx = x - cx;
      const double dy = y - cy;
      if ((dx * dx + dy * dy) <= cr2)
      {
        ++res_all;
        ++res_is;
      }
    }
    return {res_all, res_is};
  }

  std::vector< FastCircle > fast_figures(fig_size);
  for (std::size_t j = 0; j < fig_size; ++j)
  {
    const double r = static_cast< double >(figures[j].getRadius());
    fast_figures[j] = {
      static_cast< double >(figures[j].getX()),
      static_cast< double >(figures[j].getY()),
      r * r
    };
  }
  const FastCircle *fig_ptr = fast_figures.data();

  for (std::size_t i = 0; i < tries; ++i)
  {
    std::uint64_t p1 = s * 16807ULL;
    s = (p1 & 0x7FFFFFFFULL) + (p1 >> 31);
    if (s >= 2147483647ULL) s -= 2147483647ULL;
    const double r1 = static_cast< double >(s - 1ULL);

    std::uint64_t p2 = s * 16807ULL;
    s = (p2 & 0x7FFFFFFFULL) + (p2 >> 31);
    if (s >= 2147483647ULL) s -= 2147483647ULL;
    const double r2 = static_cast< double >(s - 1ULL);

    const double x = min_x + (r1 + r2 * base) * scale_x;

    std::uint64_t p3 = s * 16807ULL;
    s = (p3 & 0x7FFFFFFFULL) + (p3 >> 31);
    if (s >= 2147483647ULL) s -= 2147483647ULL;
    const double r3 = static_cast< double >(s - 1ULL);

    std::uint64_t p4 = s * 16807ULL;
    s = (p4 & 0x7FFFFFFFULL) + (p4 >> 31);
    if (s >= 2147483647ULL) s -= 2147483647ULL;
    const double r4 = static_cast< double >(s - 1ULL);

    const double y = min_y + (r3 + r4 * base) * scale_y;

    std::size_t count_fig = 0;
    for (std::size_t j = 0; j < fig_size; ++j)
    {
      const double dx = x - fig_ptr[j].x;
      const double dy = y - fig_ptr[j].y;
      if (((dx * dx) + (dy * dy)) <= fig_ptr[j].r2)
      {
        ++count_fig;
      }
    }

    if (count_fig > 0)
    {
      ++res_all;
    }
    if (count_fig == fig_size)
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
  const std::size_t base_tries = tries / threads;
  const std::size_t remainder = tries % threads;

  std::vector< std::future< count_pair_t > > results;
  if (threads > 1)
  {
    results.reserve(threads - 1);
    for (std::size_t i = 1; i < threads; ++i)
    {
      const std::size_t thread_tries = base_tries;
      const int thread_seed = seed + static_cast< int >(i);
      const auto policy = std::launch::async;
      auto task = std::async(policy, calculate, std::cref(figures), std::cref(pg), thread_tries, thread_seed);
      results.push_back(std::move(task));
    }
  }

  const count_pair_t first_res = calculate(figures, pg, base_tries + remainder, seed);
  std::size_t total_all = first_res.first;
  std::size_t total_is = first_res.second;

  for (std::size_t i = 0; i < results.size(); ++i)
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

lavrentev::Polygon lavrentev::buildBoundingBox(const f_t &figures)
{
  if (figures.empty())
  {
    return Polygon(0, 0, 0, 0);
  }

  const int first_r = figures[0].getRadius();
  const int first_x = figures[0].getX();
  const int first_y = figures[0].getY();
  Polygon pg(first_x - first_r, first_y - first_r, first_x + first_r, first_y + first_r);

  for (std::size_t i = 1; i < figures.size(); ++i)
  {
    const int r = figures[i].getRadius();
    const int x = figures[i].getX();
    const int y = figures[i].getY();

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
  }
  return pg;
}
