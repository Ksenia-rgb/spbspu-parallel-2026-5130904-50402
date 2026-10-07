#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <cstddef>
#include <string>
#include <exception>

#include "circle.hpp"
#include "polygon.hpp"
#include "calculations.hpp"

int main(int argc, char *argv[])
{
  constexpr int MIN_ARGS = 3;
  constexpr int MAX_ARGS = 4;
  constexpr int ARG_THREADS_IDX = 1;
  constexpr int ARG_TRIES_IDX = 2;
  constexpr int ARG_SEED_IDX = 3;
  constexpr int ERROR_ARGS = 1;
  constexpr int ERROR_INPUT = 2;

  if (argc < MIN_ARGS || argc > MAX_ARGS)
  {
    std::cerr << "Invalid number of arguments\n";
    return ERROR_ARGS;
  }

  size_t threads = 0;
  size_t tries = 0;
  try
  {
    threads = std::stoul(argv[ARG_THREADS_IDX]);
    tries = std::stoul(argv[ARG_TRIES_IDX]);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Invalid threads or tries\n";
    return ERROR_ARGS;
  }

  int seed = 0;
  if (argc == MAX_ARGS)
  {
    seed = std::stoi(argv[ARG_SEED_IDX]);
    if (seed < 0)
    {
      std::cerr << "Invalid seed\n";
      return ERROR_ARGS;
    }
  }

  lavrentev::Polygon pg(
    std::numeric_limits< int >::max(),
    std::numeric_limits< int >::max(),
    std::numeric_limits< int >::min(),
    std::numeric_limits< int >::min()
  );
  std::vector< lavrentev::Circle > figures;
  try
  {
    figures = lavrentev::readInput(pg);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Input processing error\n";
    return ERROR_INPUT;
  }

  const std::pair< double, double > res = lavrentev::area(figures, pg, threads, tries, seed);
  std::cout << res.first << " " << res.second << "\n";
}
