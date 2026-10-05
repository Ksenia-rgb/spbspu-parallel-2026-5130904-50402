#include <iostream>
#include <vector>
#include <future>
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
  std::pair< size_t, size_t > calculate(const std::vector< lavrentev::Circle > &figures, const lavrentev::Polygon &pg, size_t tries, int seed);
  size_t countInside(const std::vector< lavrentev::Circle > &figures, double x, double y);
  std::pair< double, double > area(const std::vector< lavrentev::Circle > &figures,
    const lavrentev::Polygon &pg,
    size_t threads,
    size_t tries,
    int seed
  );
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    std::cerr << "Invalid number of arguments\n";
    return 1;
  }

  size_t threads;
  size_t tries;
  try
  {
    threads = std::stoul(argv[1]);
    tries = std::stoul(argv[2]);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Invalid threads or tries\n";
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

  std::pair< double, double > res = lavrentev::area(figures, pg, threads, tries, seed);
  std::cout << res.first << " " << res.second << "\n";
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

std::pair< size_t, size_t > lavrentev::calculate(const std::vector< lavrentev::Circle > &figures, const lavrentev::Polygon &pg, size_t tries, int seed)
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
    size_t countFig = countInside(figures, x, y);
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

std::pair< double, double > lavrentev::area(const std::vector< lavrentev::Circle > &figures,
  const lavrentev::Polygon &pg,
  size_t threads,
  size_t tries,
  int seed)
{
  std::vector< std::future< std::pair< size_t, size_t > > > results;
  results.reserve(threads);
  size_t base_tries = tries / threads;
  size_t remainder = tries % threads;

  for (size_t i = 0; i < threads; ++i)
  {
    size_t thread_tries = base_tries + (i == 0 ? remainder : 0);

    results.push_back(std::async(
      std::launch::async,
      lavrentev::calculate,
      std::cref(figures),
      std::cref(pg),
      thread_tries,
      seed
    ));
  }

  size_t totalAll = 0;
  size_t totalIS = 0;
  for (size_t i = 0; i < threads; ++i)
  {
    std::pair< double, double > res = results[i].get();
    totalAll += res.first;
    totalIS += res.second;
  }

  double pgWidth = static_cast< double >(pg.getMaxX() - pg.getMinX());
  double pgHeight = static_cast< double >(pg.getMaxY() - pg.getMinY());
  double totalArea = pgWidth * pgHeight;

  double all = totalArea * static_cast< double >(totalAll) / static_cast< double >(tries);
  double is = totalArea * static_cast< double >(totalIS) / static_cast< double >(tries);

  return {all, is};
}
