#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "area.hpp"
#include "shapes.hpp"

int main(int argc, char *argv[])
{
  constexpr int args_wo_seed = 3;
  constexpr int args_w_seed = 4;

  constexpr int arg_threads_pos = 1;
  constexpr int arg_tries_pos = 2;
  constexpr int arg_seed_pos = 3;

  constexpr int status_error_invalid_args = 1;
  constexpr int status_error_invalid_input = 2;

  constexpr int max_threads = 1000;

  if ((argc < args_wo_seed) || (argc > args_w_seed))
  {
    std::cerr << "Invalid number of arguments\n";
    return status_error_invalid_args;
  }

  std::size_t threads = 1;
  std::size_t tries = 0;
  try
  {
    threads = std::stoul(argv[arg_threads_pos]);
    tries = std::stoul(argv[arg_tries_pos]);
  }
  catch (const std::exception &)
  {
    std::cerr << "Invalid threads or tries\n";
    return status_error_invalid_args;
  }

  if (tries == 0)
  {
    std::cerr << "Invalid tries\n";
    return status_error_invalid_args;
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
  if (argc == args_w_seed)
  {
    try
    {
      seed = std::stoi(argv[arg_seed_pos]);
    }
    catch (const std::exception &)
    {
      std::cerr << "Invalid seed\n";
      return status_error_invalid_args;
    }

    if (seed < 0)
    {
      std::cerr << "Invalid seed\n";
      return status_error_invalid_args;
    }
  }

  const int max_coord = std::numeric_limits< int >::max();
  const int min_coord = std::numeric_limits< int >::min();
  novikov::Polygon pg(max_coord, max_coord, min_coord, min_coord);
  std::vector< novikov::Circle > figures;
  try
  {
    figures = novikov::readInput(pg);
  }
  catch (const std::exception &)
  {
    std::cerr << "Input processing error\n";
    return status_error_invalid_input;
  }

  const std::pair< double, double > res = novikov::area(figures, pg, threads, tries, seed);
  std::cout << res.first << " " << res.second << "\n";
}
