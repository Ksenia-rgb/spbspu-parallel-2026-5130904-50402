#include <iostream>
#include <string>
#include <cstddef>
#include "Point.hpp"

int main(int argc, char** argv)
{
  if (argc > 4 || argc < 3)
  {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }

  int threads = std::stoi(argv[1]);
  size_t tries = std::stoull(argv[2]);

  if (threads <= 0 || tries == 0)
  {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }

  size_t seed = 0;

  if (argc == 4)
  {
    seed = std::stoull(argv[3]);
  }
}
