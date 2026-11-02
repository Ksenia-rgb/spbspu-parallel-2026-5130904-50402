#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "calculations.hpp"
#include "circle.hpp"
#include "polygon.hpp"

int main(int argc, char *argv[])
{
  constexpr int min_args = 3;
  constexpr int max_args = 4;
  constexpr int arg_threads_idx = 1;
  constexpr int arg_tries_idx = 2;
  constexpr int arg_seed_idx = 3;
  constexpr int error_args = 1;
  constexpr int error_input = 2;
  constexpr int max_threads = 1000;

  if ((argc < min_args) || (argc > max_args))
  {
    std::cerr << "Invalid number of arguments\n";
    return error_args;
  }

  std::size_t threads = 1;
  std::size_t tries = 0;
  try
  {
    threads = std::stoul(argv[arg_threads_idx]);
    tries = std::stoul(argv[arg_tries_idx]);
  }
  catch (const std::exception &)
  {
    std::cerr << "Invalid threads or tries\n";
    return error_args;
  }

  if (tries == 0)
  {
    std::cerr << "Invalid tries\n";
    return error_args;
  }
  if (threads == 0)
  {
    threads = 1;
  }
  if (threads > max_threads)
  {
    threads = max_threads;
  }

  int seed = 0;
  if (argc == max_args)
  {
    try
    {
      seed = std::stoi(argv[arg_seed_idx]);
    }
    catch (const std::exception &)
    {
      std::cerr << "Invalid seed\n";
      return error_args;
    }

    if (seed < 0)
    {
      std::cerr << "Invalid seed\n";
      return error_args;
    }
  }

  const int max_coord = std::numeric_limits< int >::max();
  const int min_coord = std::numeric_limits< int >::min();
  lavrentev::Polygon pg(max_coord, max_coord, min_coord, min_coord);
  std::vector< lavrentev::Circle > figures;
  try
  {
    figures = lavrentev::readInput(pg);
  }
  catch (const std::exception &)
  {
    std::cerr << "Input processing error\n";
    return error_input;
  }

  const std::pair< double, double > res = lavrentev::area(figures, pg, threads, tries, seed);
  std::cout << res.first << " " << res.second << "\n";
}
