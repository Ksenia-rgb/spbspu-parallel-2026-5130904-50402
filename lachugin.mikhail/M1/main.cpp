#include <iostream>

namespace lachugin
{
  struct Circle
  {
    int r;
    int x;
    int y;
  };
}

int main(int argc, char* argv[])
{
  if (argc != 3 && argc != 4)
  {
    std::cerr << "err in parametrs\n";
    return 1;
  }

  int seed = 0;
  size_t posSeed = 0;
  int threads = 0;
  size_t posThreads = 0;
  int tries = 0;
  size_t posTries = 0;
  try
  {
    if (argc == 4)
    {
      seed = std::stoi(argv[3], &posSeed);
      if (posSeed != std::string(argv[3]).length() || seed < 0)
      {
        throw std::invalid_argument("wrong seed");
      }
    }

    threads = std::stoi(argv[1], &posThreads);
    tries = std::stoi(argv[2], &posTries);
    if (posThreads != std::string(argv[1]).length() || threads < 0
      || posTries != std::string(argv[2]).length() || tries <= 0)
    {
      throw std::invalid_argument (" wrong parametr");
    }

  } catch (const std::exception& err)
  {
    std::cerr << err.what() << "\n";
    return 1;
  }
}
