#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <utility>
#include <limits>

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
    int getX() const { return dx; }
    int getY() const { return dy; }
    int getRadius() const { return radius; }
  };

  class Polygon
  {
    int maxX, maxY;
    int minX, minY;

    public:
    Polygon(int x1, int y1, int x2, int y2)
    {
      minX = x1;
      minY = y1;
      maxX = x2;
      maxY = y2;
    }
    int getMaxX() const { return maxX; }
    int getMaxY() const { return maxY; }
    int getMinX() const { return minX; }
    int getMinY() const { return minY; }
    void setMaxX(int x) { maxX = x; }
    void setMaxY(int y) { maxY = y; }
    void setMinX(int x) { minX = x; }
    void setMinY(int y) { minY = y; }
  };

  std::vector< lavrentev::Circle > readInput(lavrentev::Polygon& pg);
  std::pair< size_t, size_t > calculate(const std::vector< lavrentev::Circle > &figures, const lavrentev::Polygon &pg, int tries, int seed);
  size_t countInside(const std::vector< lavrentev::Circle > &figures, double x, double y);
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    std::cerr << "Invalid number of arguments\n";
    return 1;
  }

  int threads;
  int tries;
  try
  {
    threads = std::stoi(argv[1]);
    tries = std::stoi(argv[2]);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Invalid threads or tries\n";
    return 1;
  }
  if (threads <= 0 || tries <= 0)
  {
    std::cerr << "Invalid number of threads or tries\n";
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

  lavrentev::Polygon pg(
    std::numeric_limits<int>::max(),
    std::numeric_limits<int>::max(),
    std::numeric_limits<int>::min(),
    std::numeric_limits<int>::min()
  );
  std::vector< lavrentev::Circle > figures;
  try
  {
    figures = lavrentev::readInput(pg);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Input processing error" << "\n";
    return 2;
  }

  for (size_t i = 0; i < threads; ++i)
  {

  }
}

std::vector< lavrentev::Circle > lavrentev::readInput(lavrentev::Polygon& pg)
{
  std::vector< lavrentev::Circle > figures;
  int r, a, x, y;
  while(std::cin >> r)
  {
    if (!(std::cin >> a >> x >> y))
    {
      throw std::runtime_error("Invalid figure parameters");
    }

    if (x - r < pg.getMinX()) pg.setMinX(x - r);
    if (y - r < pg.getMinY()) pg.setMinY(y - r);
    if (x + r > pg.getMaxX()) pg.setMaxX(x + r);
    if (y + r > pg.getMaxY()) pg.setMaxY(y + r);

    figures.push_back(lavrentev::Circle{r, x, y});
  }
  if (!std::cin.eof() || figures.empty()) {
    throw std::runtime_error("Invalid input");
  }
  return figures;
}

std::pair< size_t, size_t > lavrentev::calculate(const std::vector< lavrentev::Circle > &figures, const lavrentev::Polygon &pg, int tries, int seed)
{
  std::default_random_engine engine(seed);

  std::uniform_real_distribution< double > distX(pg.getMinX(), pg.getMaxX());
  std::uniform_real_distribution< double > distY(pg.getMinY(), pg.getMaxY());

  size_t resAll = 0;
  size_t resIS = 0;
  for (size_t i = 0; i < tries; ++i)
  {
    double x = distX(engine);
    double y = distY(engine);
    int countFig = countInside(figures, x, y);
    if (countFig > 0)
    {
      ++resAll;
    }
    if (countFig == figures.size())
    {
      ++resIS;
    }
  }
  return {resAll, resIS};
}

size_t lavrentev::countInside(const std::vector< lavrentev::Circle > &figures, double x, double y)
{
  size_t res = 0;
  for (const auto &circle : figures)
  {
    if ((x - circle.getX()) * (x - circle.getX()) + (y - circle.getY()) * (y - circle.getY())
      <= circle.getRadius() * circle.getRadius())
    {
      ++res;
    }
  }
  return res;
}
