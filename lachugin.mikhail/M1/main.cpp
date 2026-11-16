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
  struct Figure
  {
    int a;
    int b;
    int x;
    int y;
  };

  struct ThreadData
  {
    std::thread worker;
    size_t u_count = 0;
    size_t i_count = 0;
  };

  Figure* newFigures(size_t size, size_t& cap, const Figure* old_figures)
  {
    const size_t new_cap = cap * 2;
    Figure* const res = new Figure[new_cap];

    for (size_t i = 0; i < size; ++i)
    {
      res[i] = old_figures[i];
    }

    cap = new_cap;

    return res;
  }

  void readFigures(Figure*& figures, size_t& cap, size_t& size)
  {
    int a = 0;
    int b = 0;
    int x = 0;
    int y = 0;

    while (true)
    {
      std::cin >> a;
      if (std::cin.eof())
      {
        break;
      }

      if (std::cin.fail())
      {
        throw std::invalid_argument("wrong data");
      }

      std::cin >> b >> x >> y;

      if (std::cin.eof() || std::cin.fail())
      {
        throw std::invalid_argument("wrong data");
      }

      if (a <= 0 || b < 0)
      {
        throw std::invalid_argument("wrong figure parameters");
      }

      if (cap == size)
      {
        Figure* const figure_new = newFigures(size, cap, figures);
        delete[] figures;
        figures = figure_new;
      }

      figures[size].a = a;
      figures[size].b = b;
      figures[size].x = x;
      figures[size].y = y;

      size++;
    }
  }

  bool isInsideFigure(const Figure& figure, double x, double y)
  {
    const double horizontal_radius = figure.a;
    const double vertical_radius = figure.b == 0 ? horizontal_radius : figure.b;

    const double dx = x - figure.x;
    const double dy = y - figure.y;

    return (dx * dx) / (horizontal_radius * horizontal_radius)
        + (dy * dy) / (vertical_radius * vertical_radius) <= 1.0;
  }

  bool isInsideUnion(const Figure* figures, size_t size, double x, double y)
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (isInsideFigure(figures[i], x, y))
      {
        return true;
      }
    }

    return false;
  }

  bool isInsideIntersection(const Figure* figures, size_t size, double x, double y)
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (!isInsideFigure(figures[i], x, y))
      {
        return false;
      }
    }

    return true;
  }

  void findBounds(const Figure* figures, size_t size, double& min_x, double& max_x, double& min_y, double& max_y)
  {
    const double first_horizontal_radius = figures[0].a;
    const double first_vertical_radius = figures[0].b == 0 ? first_horizontal_radius : figures[0].b;

    min_x = figures[0].x - first_horizontal_radius;
    max_x = figures[0].x + first_horizontal_radius;
    min_y = figures[0].y - first_vertical_radius;
    max_y = figures[0].y + first_vertical_radius;

    for (size_t i = 1; i < size; ++i)
    {
      const double horizontal_radius = figures[i].a;
      const double vertical_radius = figures[i].b == 0 ? horizontal_radius : figures[i].b;

      if (figures[i].x - horizontal_radius < min_x)
      {
        min_x = figures[i].x - horizontal_radius;
      }

      if (figures[i].x + horizontal_radius > max_x)
      {
        max_x = figures[i].x + horizontal_radius;
      }

      if (figures[i].y - vertical_radius < min_y)
      {
        min_y = figures[i].y - vertical_radius;
      }

      if (figures[i].y + vertical_radius > max_y)
      {
        max_y = figures[i].y + vertical_radius;
      }
    }
  }

  std::pair< double, double > generatePoint(
      std::default_random_engine& generator, double min_x, double max_x, double min_y, double max_y)
  {
    std::uniform_real_distribution< double > distr_x(min_x, max_x);
    const double x = distr_x(generator);

    std::uniform_real_distribution< double > distr_y(min_y, max_y);
    const double y = distr_y(generator);

    return {x, y};
  }

  void threadFunction(const Figure* figures, size_t size, size_t tries, size_t thread_number, int seed, double min_x,
      double max_x, double min_y, double max_y, size_t& u_count, size_t& i_count);

  void calculateCounts(const Figure* figures, size_t size, size_t tries, std::default_random_engine& generator,
      double min_x, double max_x, double min_y, double max_y, size_t& u_count, size_t& i_count);

  std::pair< double, double > calculateAreas(const Figure* figures, size_t size, size_t tries, int threads, int seed)
  {
    double min_x = 0;
    double max_x = 0;
    double min_y = 0;
    double max_y = 0;

    findBounds(figures, size, min_x, max_x, min_y, max_y);

    const double area = (max_x - min_x) * (max_y - min_y);

    if (threads == 0)
    {
      size_t u_count = 0;
      size_t i_count = 0;

      std::default_random_engine generator(seed);
      calculateCounts(figures, size, tries, generator, min_x, max_x, min_y, max_y, u_count, i_count);

      const double area_u = area * (static_cast< double >(u_count) / tries);
      const double area_i = area * (static_cast< double >(i_count) / tries);

      return {area_u, area_i};
    }

    const size_t thread_count = static_cast< size_t >(threads);
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

        data[i].worker = std::thread(threadFunction, figures, size, current_tries, i, seed, min_x, max_x, min_y, max_y,
            std::ref(data[i].u_count), std::ref(data[i].i_count));
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

    const double area_u = area * (static_cast< double >(total_u) / tries);
    const double area_i = area * (static_cast< double >(total_i) / tries);

    return {area_u, area_i};
  }

  void calculateCounts(const Figure* figures, size_t size, size_t tries, std::default_random_engine& generator,
      double min_x, double max_x, double min_y, double max_y, size_t& u_count, size_t& i_count)
  {
    for (size_t i = 0; i < tries; ++i)
    {
      const std::pair< double, double > point = generatePoint(generator, min_x, max_x, min_y, max_y);

      if (isInsideUnion(figures, size, point.first, point.second))
      {
        u_count++;
      }

      if (isInsideIntersection(figures, size, point.first, point.second))
      {
        i_count++;
      }
    }
  }

  void threadFunction(const Figure* figures, size_t size, size_t tries, size_t thread_number, int seed, double min_x,
      double max_x, double min_y, double max_y, size_t& u_count, size_t& i_count)
  {
    std::default_random_engine generator(seed + thread_number);

    calculateCounts(figures, size, tries, generator, min_x, max_x, min_y, max_y, u_count, i_count);
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

    if (pos_threads != std::string(argv[1]).length() || threads < 0 || pos_tries != std::string(argv[2]).length()
        || tries <= 0)
    {
      throw std::invalid_argument("wrong parametr");
    }
  }
  catch (const std::exception& err)
  {
    std::cerr << err.what() << "\n";
    return 1;
  }

  lachugin::Figure* figures = nullptr;
  size_t size = 0;
  size_t cap = 2;

  figures = new lachugin::Figure[cap];

  try
  {
    lachugin::readFigures(figures, cap, size);
  }
  catch (const std::invalid_argument& err)
  {
    std::cerr << err.what() << "\n";
    delete[] figures;
    return 1;
  }
  catch (const std::bad_alloc& err)
  {
    std::cerr << err.what() << "\n";
    delete[] figures;
    return 1;
  }

  try
  {
    const std::pair< double, double > result = lachugin::calculateAreas(figures, size, tries, threads, seed);

    std::cout << std::setprecision(std::numeric_limits< double >::max_digits10);
    std::cout << result.first << "\n";
    std::cout << result.second << "\n";
  }
  catch (const std::exception& err)
  {
    std::cerr << err.what() << "\n";
    delete[] figures;
    return 1;
  }

  delete[] figures;
}
