#include <iostream>
#include <vector>
#include <cerrno>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <limits>
#include <thread>
#include <numeric>
#include <cstddef>
#include <random>
#include "Shape.hpp"

namespace afanasev
{
  bool parseArg(const char * s, long long & out)
  {
    char * end = nullptr;
    errno = 0;
    const long long v = std::strtoll(s, &end, 10);

    if (errno == ERANGE || end == s || *end != '\0' || v < 0)
    {
      return false;
    }
    out = v;
    return true;
  }
}

int main(int argc, char ** argv)
{
  namespace av = afanasev;

  if (argc != 3 && argc != 4)
  {
    std::cerr << "Usage: " << argv[0] << " threads tries [seed]\n";
    return 1;
  }

  long long threads = 0;
  long long tries = 0;
  long long seed = 0;

  if (!av::parseArg(argv[1], threads) || !av::parseArg(argv[2], tries) || (argc == 4 && !av::parseArg(argv[3], seed)))
  {
    std::cerr << "invalid command line argument\n";
    return 1;
  }

  threads = threads ? threads : 1;

  if (tries == 0)
  {
    std::cerr << "tries must be positive\n";
    return 1;
  }

  std::vector< av::Shape > shapes;

  long long r = 0;
  long long second = 0;
  long long x = 0;
  long long y = 0;

  while (std::cin >> r)
  {
    if (!(std::cin >> second >> x >> y))
    {
      std::cerr << "invalid figure input\n";
      return 1;
    }

    try
    {
      shapes.emplace_back(r, second, x, y);
    }
    catch (const std::exception & e)
    {
      std::cerr << e.what() << '\n';
      return 1;
    }
  }

  if (!std::cin.eof())
  {
    std::cerr << "invalid figure input\n";
    return 1;
  }

  if (shapes.empty())
  {
    std::cout << std::setprecision(std::numeric_limits< double >::max_digits10);
    std::cout << 0.0 << ' ' << 0.0 << '\n';
    return 0;
  }

  double min_x = std::numeric_limits< double >::infinity();
  double max_x = -std::numeric_limits< double >::infinity();
  double min_y = std::numeric_limits< double >::infinity();
  double max_y = -std::numeric_limits< double >::infinity();

  for (const av::Shape & s : shapes)
  {
    s.extendBBox(min_x, max_x, min_y, max_y);
  }

  const double bbox_area = (max_x - min_x) * (max_y - min_y);

  const std::size_t nthreads = static_cast< std::size_t >(threads);

  std::vector< long long > union_counts(nthreads, 0);
  std::vector< long long > inter_counts(nthreads, 0);
  std::vector< std::thread > workers;
  workers.reserve(nthreads);

  const long long base = tries / static_cast< long long >(nthreads);
  const long long rem = tries % static_cast< long long >(nthreads);

  const unsigned base_seed = static_cast< unsigned >(seed);

  for (std::size_t t = 0; t < nthreads; ++t)
  {
    const long long cnt = base + (static_cast< long long >(t) < rem ? 1LL : 0LL);
    const unsigned thread_seed = base_seed + static_cast< unsigned >(t);

    workers.emplace_back(
        [&, t, cnt, thread_seed]()
        {
          std::default_random_engine gen(thread_seed);

          std::uniform_real_distribution< double > dist_x(min_x, max_x);
          std::uniform_real_distribution< double > dist_y(min_y, max_y);

          long long in_union = 0;
          long long in_inter = 0;

          for (long long i = 0; i < cnt; ++i)
          {
            const double px = dist_x(gen);
            const double py = dist_y(gen);

            bool any = false;
            bool all = true;

            for (const av::Shape & s : shapes)
            {
              const bool inside = s.contains(px, py);
              any = any || inside;
              all = all && inside;
            }

            if (any)
            {
              ++in_union;
            }
            if (all)
            {
              ++in_inter;
            }
          }

          union_counts[t] = in_union;
          inter_counts[t] = in_inter;
        });
  }

  for (std::thread & w : workers)
  {
    w.join();
  }

  const long long total_union = std::accumulate(union_counts.begin(), union_counts.end(), 0LL);
  const long long total_inter = std::accumulate(inter_counts.begin(), inter_counts.end(), 0LL);

  const double union_area = bbox_area * static_cast< double >(total_union) / static_cast< double >(tries);
  const double inter_area = bbox_area * static_cast< double >(total_inter) / static_cast< double >(tries);

  std::cout << std::setprecision(std::numeric_limits< double >::max_digits10);
  std::cout << union_area << ' ' << inter_area << '\n';

  return 0;
}
