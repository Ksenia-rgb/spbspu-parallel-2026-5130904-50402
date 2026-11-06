#include <iostream>

namespace lachugin
{
  struct Circle
  {
    int r;
    int x;
    int y;
  };

  Circle* newCircles(size_t size, size_t& cap, const Circle* oldCircles)
  {
    size_t newCap = cap * 2;
    Circle* res = new Circle[newCap];

    for (size_t i = 0; i < size; ++i)
    {
      res[i] = oldCircles[i];
    }
    cap = newCap;

    return res;
  }
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

  int r = 0;
  int ign = 0;
  int x = 0;
  int y = 0;

  lachugin::Circle* circles = nullptr;
  size_t size = 0;
  size_t cap = 2;
  circles = new lachugin::Circle[cap];

  while (true)
  {
    std::cin >> r;
    if (std::cin.eof())
    {
      break;
    }
    if (std::cin.fail())
    {
      std::cerr << ("wrong data\n");
      delete[] circles;
      return 1;
    }
    std::cin >> ign >> x >> y;
    if (std::cin.eof() || std::cin.fail())
    {
      std::cerr << ("wrong data\n");
      delete[] circles;
      return 1;
    }

    try
    {
      if (cap == size)
      {
        lachugin::Circle* circleNew = nullptr;
        circleNew = lachugin::newCircles(size, cap, circles);
        delete[] circles;
        circles = circleNew;
      }
    } catch (const std::bad_alloc& err)
    {
      std::cerr << err.what();
      delete[] circles;
      return 1;
    }

    circles[size].r = r;
    circles[size].x = x;
    circles[size].y = y;

    size++;


  }


  delete[] circles;



}
