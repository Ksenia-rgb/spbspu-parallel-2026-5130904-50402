#include "monte-carlo.hpp"
#include <algorithm>
#include <random>

makarov::Hits makarov::countHits(
    const std::vector< Circle >& circles, const Box& box, std::size_t tries, std::size_t seed)
{
  std::mt19937 gen(static_cast< std::mt19937::result_type >(seed));
  std::uniform_real_distribution< double > getX(box.left, box.right);
  std::uniform_real_distribution< double > getY(box.bottom, box.top);
  Hits hits{0, 0};
  for (std::size_t i = 0; i < tries; ++i) {
    const double x = getX(gen);
    const double y = getY(gen);
    auto inside = [x, y](const Circle& c)
    {
      return isInside(c, x, y);
    };
    if (std::any_of(circles.begin(), circles.end(), inside)) {
      ++hits.any;
    }
    if (std::all_of(circles.begin(), circles.end(), inside)) {
      ++hits.all;
    }
  }
  return hits;
}
