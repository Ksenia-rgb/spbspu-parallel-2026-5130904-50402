#include <iostream>
#include <random>
#include <utility>
#include <functional>
#include <iomanip>
#include <thread>
#include <limits>
#include <string>
#include <stdexcept>
#include <new>

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
    size_t u_count = 0;
    size_t i_count = 0;
  };

  Circle* newCircles(size_t size, size_t& cap, const Circle* old_circles)
  {
    const size_t new_cap = cap * 2;
    Circle* const res = new Circle[new_cap];

    for (size_t i = 0; i < size; ++i)
    {
      res[i] = old_circles[i];
    }

    cap = new_cap;

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
        Circle* const circle_new = newCircles(size, cap, circles);
        delete[] circles;
        circles = circle_new;
      }

      circles[size].r = r;
      circles[size].x = x;
      circles[size].y = y;

      size++;
    }
  }

  bool isInsideCircle(const Circle& circle, double x, double y)
  {
    return ((x - circle.x) * (x - circle.x)) +
        ((y - circle.y) * (y - circle.y)) <= circle.r * circle.r;
  }

  bool isInsideUnion(const Circle* circles, size_t size, double x, double y)
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (isInsideCircle(circles[i], x, y))
      {
        return true;
      }
    }

    return false;
  }

  bool isInsideIntersection(const Circle* circles, size_t size, double x, double y)
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (!isInsideCircle(circles[i], x, y))
      {
        return false;
      }
    }

    return true;
  }

  void findBounds(const Circle* circles, size_t size, double& min_x, double& max_x,
      double& min_y, double& max_y)
  {
    min_x = circles[0].x - circles[0].r;
    max_x = circles[0].x + circles[0].r;

    min_y = circles[0].y - circles[0].r;
    max_y = circles[0].y + circles[0].r;

    for (size_t i = 1; i < size; ++i)
    {
      if (circles[i].x - circles[i].r < min_x)
      {
        min_x = circles[i].x - circles[i].r;
      }

      if (circles[i].x + circles[i].r > max_x)
      {
        max_x = circles[i].x + circles[i].r;
      }

      if (circles[i].y - circles[i].r < min_y)
      {
        min_y = circles[i].y - circles[i].r;
      }

      if (circles[i].y + circles[i].r > max_y)
      {
        max_y = circles[i].y + circles[i].r;
      }
    }
  }

  std::pair<double, double> generatePoint(std::default_random_engine& generator,
      double min_x, double max_x, double min_y, double max_y)
  {
    std::uniform_real_distribution<double> distr_x(min_x, max_x);
    const double x = distr_x(generator);

    std::uniform_real_distribution<double> distr_y(min_y, max_y);
    const double y = distr_y(generator);

    return {x, y};
  }

  void threadFunction(const Circle* circles, size_t size, size_t tries,
      size_t thread_number, int seed, double min_x, double max_x,
      double min_y, double max_y, size_t& u_count, size_t& i_count);

  void calculateCounts(const Circle* circles, size_t size, size_t tries,
      std::default_random_engine& generator, double min_x, double max_x,
      double min_y, double max_y, size_t& u_count, size_t& i_count);

  std::pair<double, double> calculateAreas(const Circle* circles,
      size_t size, size_t tries, int threads, int seed)
  {
    double min_x = 0;
    double max_x = 0;
    double min_y = 0;
    double max_y = 0;

    findBounds(circles, size, min_x, max_x, min_y, max_y);

    const double area = (max_x - min_x) * (max_y - min_y);

    if (threads == 0)
    {
      size_t u_count = 0;
      size_t i_count = 0;

      std::default_random_engine generator(seed);

      calculateCounts(circles, size, tries, generator,
          min_x, max_x, min_y, max_y, u_count, i_count);

      const double area_u = area * (static_cast<double>(u_count) / tries);
      const double area_i = area * (static_cast<double>(i_count) / tries);

      return {area_u, area_i};
    }

    const size_t thread_count = static_cast<size_t>(threads);
    const size_t tries_per_thread = tries / thread_count;
    const size_t remainder = tries % thread_count;

    ThreadData* const data = new ThreadData[thread_count];
    size_t started = 0;

    try
    {
      for (size_t i = 0; i < thread_count; ++i)
      {
        size_t current_tries = tries_per_thread;

        if (i == thread_count - 1)
        {
          current_tries += remainder;
        }

        data[i].worker = std::thread(threadFunction,
            circles, size, current_tries, i, seed,
            min_x, max_x, min_y, max_y,
            std::ref(data[i].u_count),
            std::ref(data[i].i_count));

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

    size_t total_u = 0;
    size_t total_i = 0;

    for (size_t i = 0; i < thread_count; ++i)
    {
      data[i].worker.join();

      total_u += data[i].u_count;
      total_i += data[i].i_count;
    }

    delete[] data;

    const double area_u = area * (static_cast<double>(total_u) / tries);
    const double area_i = area * (static_cast<double>(total_i) / tries);

    return {area_u, area_i};
  }

  void calculateCounts(const Circle* circles, size_t size, size_t tries,
      std::default_random_engine& generator, double min_x, double max_x,
      double min_y, double max_y, size_t& u_count, size_t& i_count)
  {
    for (size_t i = 0; i < tries; ++i)
    {
      const std::pair<double, double> point =
          generatePoint(generator, min_x, max_x, min_y, max_y);

      if (isInsideUnion(circles, size, point.first, point.second))
      {
        u_count++;
      }

      if (isInsideIntersection(circles, size, point.first, point.second))
      {
        i_count++;
      }
    }
  }

  void threadFunction(const Circle* circles, size_t size, size_t tries,
      size_t thread_number, int seed, double min_x, double max_x,
      double min_y, double max_y, size_t& u_count, size_t& i_count)
  {
    std::default_random_engine generator(seed + thread_number);

    calculateCounts(circles, size, tries, generator,
        min_x, max_x, min_y, max_y, u_count, i_count);
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
  size_t pos_seed = 0;

  int threads = 0;
  size_t pos_threads = 0;

  int tries = 0;
  size_t pos_tries = 0;

  try
  {
    if (argc == 4)
    {
      seed = std::stoi(argv[3], &pos_seed);

      if (pos_seed != std::string(argv[3]).length() || seed < 0)
      {
        throw std::invalid_argument("wrong seed");
      }
    }

    threads = std::stoi(argv[1], &pos_threads);
    tries = std::stoi(argv[2], &pos_tries);

    if (pos_threads != std::string(argv[1]).length() || threads < 0
        || pos_tries != std::string(argv[2]).length() || tries <= 0)
    {
      throw std::invalid_argument("wrong parametr");
    }
  }
  catch (const std::exception& err)
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
  }
  catch (const std::invalid_argument& err)
  {
    std::cerr << err.what() << "\n";
    delete[] circles;
    return 1;
  }
  catch (const std::bad_alloc& err)
  {
    std::cerr << err.what() << "\n";
    delete[] circles;
    return 1;
  }

  try
  {
    const std::pair<double, double> result =
        lachugin::calculateAreas(circles, size, tries, threads, seed);

    std::cout << std::setprecision(std::numeric_limits<double>::max_digits10);

    std::cout << result.first << "\n";
    std::cout << result.second << "\n";
  }
  catch (const std::exception& err)
  {
    std::cerr << err.what() << "\n";
    delete[] circles;
    return 1;
  }

  delete[] circles;
}
