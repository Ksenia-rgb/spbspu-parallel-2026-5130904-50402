#include <iostream>
#include <random>
#include <utility>
#include <functional>
#include <thread>

namespace lachugin
{
  struct Circle
  {
    int r;
    int x;
    int y;
  };

  struct ThreadData
  {
    std::thread worker;
    size_t uCount = 0;
    size_t iCount = 0;
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

  void readCircles(Circle*& circles, size_t& cap, size_t& size)
  {
    int r = 0;
    int ign = 0;
    int x = 0;
    int y = 0;

    while (true)
    {
      std::cin >> r;
      if (std::cin.eof())
      {
        break;
      }
      if (std::cin.fail())
      {
        throw std::invalid_argument("wrong data");
      }
      std::cin >> ign >> x >> y;
      if (std::cin.eof() || std::cin.fail())
      {
        throw std::invalid_argument("wrong data");
      }


      if (cap == size)
      {
        Circle* circleNew = nullptr;
        circleNew = newCircles(size, cap, circles);
        delete[] circles;
        circles = circleNew;
      }

      circles[size].r = r;
      circles[size].x = x;
      circles[size].y = y;

      size++;


    }
  }

  bool isInsideCircle(const Circle& circle, double x, double y)
  {
    return ((x-circle.x) * (x-circle.x)) + (y-circle.y) * (y-circle.y) <= circle.r * circle.r;
  }

  bool isInsideUnion(const Circle* circles, size_t size, double x, double y)
  {
    for (size_t i = 0; i < size; ++i) {
      if (isInsideCircle(circles[i], x, y)) {
        return true;
      }
    }
    return false;
  }

  bool isInsideIntersection(const Circle* circles, size_t size, double x, double y) {
    for (size_t i = 0; i < size; ++i) {
      if (!isInsideCircle(circles[i], x, y)) {
        return false;
      }
    }
    return true;
  }

  void findBounds(const Circle* circles, size_t size, double& minX, double& maxX,
    double& minY, double& maxY)
  {
    minX = circles[0].x - circles[0].r;
    maxX = circles[0].x + circles[0].r;

    minY = circles[0].y - circles[0].r;
    maxY = circles[0].y + circles[0].r;
    for (size_t i = 1; i < size; ++i)
    {
      if (circles[i].x - circles[i].r < minX)
      {
        minX = circles[i].x - circles[i].r;
      }
      if (circles[i].x + circles[i].r > maxX)
      {
        maxX = circles[i].x + circles[i].r;
      }

      if (circles[i].y - circles[i].r < minY)
      {
        minY = circles[i].y - circles[i].r;
      }
      if (circles[i].y + circles[i].r > maxY)
      {
        maxY = circles[i].y + circles[i].r;
      }
    }
  }

  std::pair< double, double > generatePoint(std::mt19937& generator,
    double minX, double maxX, double minY, double maxY)
  {
    std::uniform_real_distribution< double > distrX(minX, maxX);
    double x = distrX(generator);
    std::uniform_real_distribution< double > distrY(minY, maxY);
    double y = distrY(generator);

    return {x, y};
  }

  void threadFunction(const Circle* circles, size_t size, size_t tries, size_t threadNumber,
    int seed, double minX, double maxX, double minY, double maxY, size_t& uCount, size_t& iCount);

  void calculateCounts(const Circle* circles, size_t size, size_t tries, std::mt19937& generator,
    double minX, double maxX, double minY, double maxY, size_t& uCount, size_t& iCount);

  std::pair< double, double > calculateAreas(const Circle* circles,
    size_t size, size_t tries, int threads, int seed)
{
  double minX = 0;
  double maxX = 0;
  double minY = 0;
  double maxY = 0;

  findBounds(circles, size, minX, maxX, minY, maxY);

  double area = (maxX - minX) * (maxY - minY);
  if (threads == 0)
  {
    size_t uCount = 0;
    size_t iCount = 0;
    std::mt19937 generator(seed);

    calculateCounts(circles, size, tries, generator,
      minX, maxX, minY, maxY, uCount, iCount);
    double areaU = area * (static_cast< double >(uCount) / tries);
    double areaI = area * (static_cast< double >(iCount) / tries);
    return {areaU, areaI};
  }
  size_t threadCount = static_cast< size_t >(threads);
  size_t triesPerThread = tries / threadCount;
  size_t remainder = tries % threadCount;
  ThreadData* data = new ThreadData[threadCount];
  size_t started = 0;

  try
  {
    for (size_t i = 0; i < threadCount; ++i)
    {
      size_t currentTries = triesPerThread;
      if (i == threadCount - 1)
      {
          currentTries += remainder;
      }
      data[i].worker = std::thread(threadFunction,
        circles, size, currentTries, i, seed,
        minX, maxX, minY, maxY, std::ref(data[i].uCount),
        std::ref(data[i].iCount));
      started++;
    }
  }
  catch (...)
  {
    for (size_t i = 0; i < started; ++i)
    {
      data[i].worker.join();
    }

    delete[] data;
    throw;
  }
  size_t totalU = 0;
  size_t totalI = 0;
  for (size_t i = 0; i < threadCount; ++i)
  {
    data[i].worker.join();
    totalU += data[i].uCount;
    totalI += data[i].iCount;
  }

  delete[] data;

  double areaU = area * (static_cast< double >(totalU) / tries);
  double areaI = area * (static_cast< double >(totalI) / tries);
  return {areaU, areaI};
}


  void calculateCounts(const Circle* circles, size_t size, size_t tries, std::mt19937& generator,
    double minX, double maxX, double minY, double maxY, size_t& uCount, size_t& iCount)
  {
    for (size_t i = 0; i < tries; ++i)
    {
      std::pair< double, double > point;
      point = generatePoint(generator, minX, maxX, minY, maxY);

      if (isInsideUnion(circles, size, point.first, point.second))
      {
        uCount++;
      }
      if (isInsideIntersection(circles, size, point.first, point.second)) {
        iCount++;
      }
    }
  }

  void threadFunction(const Circle* circles, size_t size, size_t tries, size_t threadNumber,
    int seed, double minX, double maxX, double minY, double maxY, size_t& uCount, size_t& iCount)
  {
    std::mt19937 generator(seed + threadNumber);

    calculateCounts(circles, size, tries, generator, minX, maxX, minY, maxY, uCount, iCount);
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


  lachugin::Circle* circles = nullptr;
  size_t size = 0;
  size_t cap = 2;
  circles = new lachugin::Circle[cap];

  try
  {
    lachugin::readCircles(circles, cap, size);
  } catch (const std::invalid_argument& err)
  {
    std::cerr << err.what();
    delete[] circles;
    return 1;
  }
  catch (const std::bad_alloc& err)
  {
    std::cerr << err.what();
    delete[] circles;
    return 1;
  }


  delete[] circles;



}

