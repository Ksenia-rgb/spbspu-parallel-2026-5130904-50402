#include "monte-carlo.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>
#include "shapes.hpp"

namespace
{
  struct scaled_ellipse_t
  {
    double x;
    double y;
    double horizontal_factor;
    double vertical_factor;
  };

  struct range_t
  {
    std::size_t begin;
    std::size_t end;
  };

  using scaled_ellipses_t = std::vector< scaled_ellipse_t >;
  using pool_t = std::vector< std::thread >;
  using counts_t = std::vector< std::size_t >;

  class Sampler
  {
  public:
    Sampler(const pozdnyakov::rectangle_t& frame, std::size_t tries, std::size_t first, std::size_t seed);

    pozdnyakov::point_t getNext();

  private:
    std::uint64_t state_;
    pozdnyakov::point_t origin_;
    double width_;
    double height_;
    std::size_t side_;
    std::size_t column_;
    std::size_t row_;
    double cell_width_;
    double cell_height_;

    std::uint64_t generate();
  };

  Sampler::Sampler(const pozdnyakov::rectangle_t& frame, std::size_t tries, std::size_t first, std::size_t seed):
    state_(seed),
    origin_(frame.left_bottom),
    width_(frame.right_top.x - frame.left_bottom.x),
    height_(frame.right_top.y - frame.left_bottom.y),
    side_(static_cast< std::size_t >(std::sqrt(static_cast< double >(tries)))),
    column_(first % side_),
    row_(first / side_),
    cell_width_(width_ / static_cast< double >(side_)),
    cell_height_(height_ / static_cast< double >(side_))
  {}

  pozdnyakov::point_t Sampler::getNext()
  {
    const int half_bits = 32;
    const std::uint64_t half_mask = 0xFFFFFFFF;
    const double scale = std::ldexp(1.0, -half_bits);
    const std::uint64_t bits = generate();
    const double u = static_cast< double >(bits & half_mask) * scale;
    const double v = static_cast< double >(bits >> half_bits) * scale;
    if (row_ >= side_)
    {
      return {origin_.x + (u * width_), origin_.y + (v * height_)};
    }
    const double x = origin_.x + ((static_cast< double >(column_) + u) * cell_width_);
    const double y = origin_.y + ((static_cast< double >(row_) + v) * cell_height_);
    ++column_;
    if (column_ == side_)
    {
      column_ = 0;
      ++row_;
    }
    return {x, y};
  }

  std::uint64_t Sampler::generate()
  {
    const std::uint64_t increment = 0x9E3779B97F4A7C15;
    const std::uint64_t first_multiplier = 0xBF58476D1CE4E5B9;
    const std::uint64_t second_multiplier = 0x94D049BB133111EB;
    const int first_shift = 30;
    const int second_shift = 27;
    const int third_shift = 31;
    state_ += increment;
    std::uint64_t result = state_;
    result = (result ^ (result >> first_shift)) * first_multiplier;
    result = (result ^ (result >> second_shift)) * second_multiplier;
    return result ^ (result >> third_shift);
  }

  scaled_ellipses_t scaleEllipses(const pozdnyakov::ellipses_t& ellipses)
  {
    scaled_ellipses_t result;
    result.reserve(ellipses.size());
    for (const pozdnyakov::ellipse_t& ellipse : ellipses)
    {
      const double horizontal = 1.0 / ellipse.horizontal_radius;
      const double vertical = 1.0 / ellipse.vertical_radius;
      result.push_back({ellipse.center.x, ellipse.center.y, horizontal, vertical});
    }
    return result;
  }

  bool isCovered(const scaled_ellipse_t* first, const scaled_ellipse_t* last, const pozdnyakov::point_t& point)
  {
    for (; first != last; ++first)
    {
      const double dx = (point.x - first->x) * first->horizontal_factor;
      const double dy = (point.y - first->y) * first->vertical_factor;
      if (((dx * dx) + (dy * dy)) <= 1.0)
      {
        return true;
      }
    }
    return false;
  }

  void countHits(const scaled_ellipses_t& ellipses, const pozdnyakov::rectangle_t& frame, std::size_t tries,
      const range_t& range, std::size_t seed, std::size_t& hits)
  {
    Sampler sampler(frame, tries, range.begin, seed);
    const scaled_ellipse_t* const first = ellipses.data();
    const scaled_ellipse_t* const last = first + ellipses.size();
    std::size_t local = 0;
    for (std::size_t i = range.begin; i < range.end; ++i)
    {
      if (isCovered(first, last, sampler.getNext()))
      {
        ++local;
      }
    }
    hits = local;
  }

  std::size_t getWorkers(std::size_t threads, std::size_t tries)
  {
    return std::min(std::max< std::size_t >(threads, 1), tries);
  }

  range_t getRange(std::size_t tries, std::size_t workers, std::size_t index)
  {
    const std::size_t share = tries / workers;
    const std::size_t extra = tries % workers;
    const std::size_t begin = (index * share) + std::min(index, extra);
    const std::size_t size = share + ((index < extra) ? 1 : 0);
    return {begin, begin + size};
  }

  void joinAll(pool_t& pool)
  {
    for (std::thread& worker : pool)
    {
      worker.join();
    }
  }
}

double pozdnyakov::computeCoverage(const ellipses_t& ellipses, std::size_t threads, std::size_t tries, std::size_t seed)
{
  if (ellipses.empty() || (tries == 0))
  {
    return 0.0;
  }
  const rectangle_t frame = getFrame(ellipses);
  const scaled_ellipses_t scaled = scaleEllipses(ellipses);
  const std::size_t workers = getWorkers(threads, tries);
  counts_t hits(workers, 0);
  pool_t pool;
  pool.reserve(workers - 1);
  try
  {
    for (std::size_t i = 1; i < workers; ++i)
    {
      const range_t range = getRange(tries, workers, i);
      pool.emplace_back(countHits, std::cref(scaled), std::cref(frame), tries, range, seed + i, std::ref(hits[i]));
    }
    countHits(scaled, frame, tries, getRange(tries, workers, 0), seed, hits[0]);
  }
  catch (...)
  {
    joinAll(pool);
    throw;
  }
  joinAll(pool);
  std::size_t total = 0;
  for (const std::size_t part : hits)
  {
    total += part;
  }
  const double trial = getArea(frame) / static_cast< double >(tries);
  return trial * static_cast< double >(total);
}
