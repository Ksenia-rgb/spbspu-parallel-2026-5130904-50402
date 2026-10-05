#include <iostream>

namespace lavrentev
{
  class Circle
  {
    int radius;
    int dx, dy;

    public:
    Circle(int r, int x, int y){
      radius = r;
      dx = x;
      dy = y;
    }
  };

  std::vector< lavrentev::Circle > readInput();
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    std::cerr << "Invalid number of arguments\n";
    return 1;
  }

  int threads = std::stoi(argv[1]);
  if (threads <= 0)
  {
    std::cerr << "Invalid number of threads\n";
    return 1;
  }

  int tries = std::stoi(argv[2]);
  if (tries <= 0)
  {
    std::cerr << "Invalid number of tries\n";
    return 1;
  }
  int seed = 0;
  if (argc == 4)
  {
    seed = std::stoi(argv[3]);
    if (seed < 0)
    {
      std::cerr << "Invalid seed\n";
      return 1;
    }
  }

  try
  {
    std::vector< lavrentev::Circle > figures = lavrentev::readInput();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Input processing error" << "\n";
    return 2;
  }

}

std::vector< lavrentev::Circle > readInput()
{
  std::vector< lavrentev::Circle > figures;
  while(!std::cin.eof())
  {
    int r, a, x, y;
    std::cin >> r >> a >> x >> y;
    if (std::cin.fail())
    {
      if (std::cin.eof())
      {
        break;
      }
      throw std::runtime_error("Invalid input");
    }
    figures.push_back(lavrentev::Circle{r, x, y});
  }
  return figures;
}
