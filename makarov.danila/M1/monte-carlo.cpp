#include "monte-carlo.hpp"
#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>
#include "circle.hpp"
#include "geometry.hpp"

makarov::Hits makarov::countHits(
    const std::vector< Circle >& circles, const Box& box, std::size_t tries, std::size_t seed)
{
  std::default_random_engine gen(static_cast< std::default_random_engine::result_type >(seed));
  std::uniform_real_distribution< double > get_x(box.left, box.right);
  std::uniform_real_distribution< double > get_y(box.bottom, box.top);
  Hits hits{0, 0};
  for (std::size_t i = 0; i < tries; ++i)
  {
    const double x = get_x(gen);
    const double y = get_y(gen);
    auto inside = [x, y](const Circle& c)
    {
      return isInside(c, x, y);
    };
    if (std::any_of(circles.begin(), circles.end(), inside))
    {
      ++hits.any;
    }
    if (std::all_of(circles.begin(), circles.end(), inside))
    {
      ++hits.all;
    }
  }
  return hits;
}
