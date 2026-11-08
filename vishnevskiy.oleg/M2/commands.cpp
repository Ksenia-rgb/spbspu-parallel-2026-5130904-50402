#include <iostream>
#include <string>
#include <map>
#include <random>
#include <unistd.h>
#include "commands.hpp"

namespace vishnevskiy
{
  bool isInside(double x, double y, const Circle &c)
  {
    double rx = x - c.x_;
    double ry = y - c.y_;
    double r  = c.r_;
    return (rx * rx + ry * ry) <= (r * r);
  }

  void generatePoint(std::mt19937_64 &gen, const CircleBoundaries &b, double &px, double &py)
  {
    std::uniform_real_distribution<double> distX(b.minX_, b.maxX_);
    std::uniform_real_distribution<double> distY(b.minY_, b.maxY_);
    px = distX(gen);
    py = distY(gen);
  }

  size_t calc(const std::vector<Circle> &circles, size_t tests, size_t seed, const CircleBoundaries &b)
  {
    std::mt19937_64 gen(seed);
    size_t count = 0;
    for (size_t i = 0; i < tests; ++i)
    {
      double x = 0.0;
      double y = 0.0;
      generatePoint(gen, b, x, y);
      for (size_t j = 0; j < circles.size(); ++j)
      {
        if (isInside(x, y, circles[j]))
        {
          count++;
          break;
        }
      }
    }
    return count;
  }

  void *threadStarter(void *arg)
  {
    ThreadInfo *t = static_cast<ThreadInfo *>(arg);
    t->result = calc(*t->circles, t->tests, t->seed, *t->b);
    return nullptr;
  }

  size_t runCalc(const std::vector<Circle> &circles, size_t threads, size_t tests, size_t seed, const CircleBoundaries &b)
  {
    size_t block = tests / threads;
    size_t remainder = tests % threads;
    std::vector<pthread_t> ids(threads);
    std::vector<ThreadInfo> tasks(threads);
    std::vector<bool> started(threads, false);

    for (size_t i = 0; i < threads; ++i)
    {
      size_t currTests = block;
      if (i < remainder)
      {
        currTests++;
      }
      tasks[i].circles = &circles;
      tasks[i].b = &b;
      tasks[i].tests = currTests;
      tasks[i].seed = seed;
      tasks[i].result = 0;
      int err = pthread_create(&ids[i], nullptr, threadStarter, &tasks[i]);
      if (err != 0)
      {
        tasks[i].result = calc(circles, currTests, seed, b);
        started[i] = false;
      }
      else
      {
        started[i] = true;
      }
    }

    size_t res = 0;
    for (size_t i = 0; i < threads; ++i)
    {
      if (started[i])
      {
        pthread_join(ids[i], nullptr);
      }
      res += tasks[i].result;
    }
    return res;
  }

  CircleBoundaries getBoundaries(Circle c)
  {
    CircleBoundaries b {0,0,0,0};
    b.minX_ = c.x - c.r;
    b.maxX_ = c.x + c.r;
    b.minY_ = c.y - c.r;
    b.maxY_ = c.y + c.r;
    return b;
  }

  CircleBoundaries getSetBoundaries(std::vector<Circle> &circles)
  {
    CircleBoundaries b
    {
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest(),
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest()
    };
    for (size_t i = 0; i < circles.size(); ++i)
    {
      const Circle& c = circles[i];
      double curMinX = c.x - c.r;
      double curMaxX = c.x + c.r;
      double curMinY = c.y - c.r;
      double curMaxY = c.y + c.r;
      if (curMinX < b.minX_)
      {
        b.minX_ = curMinX;
      }
      if (curMaxX > b.maxX_)
      {
        b.maxX_ = curMaxX;
      }
      if (curMinY < b.minY_)
      {
        b.minY_ = curMinY;
      }
      if (curMaxY > b.maxY_)
      {
        b.maxY_ = curMaxY;
      }
    }
    return b;
  }

  size_t writeSafe(int &err, int wr, const char *b, size_t k)
  {
    size_t r = 0;
    while (r < k)
    {
      err = write(wr, b + r, k - r);
      if (err < 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  size_t readSafe(int &err, int rd, const char *b, size_t k)
  {
    size_t r = 0;
    while (r < k)
    {
      err = read(rd, b + r, k - r);
      if (err < 0)
      {
        break;
      }
      r += err;
    }
    return r;
  }

  size_t readLine(int &err, int rd, std::string &o)
  {
    o.clear();
    err = 0;
    size_t res = 0;
    bool done = false;
    while (!done)
    {
      char ch = 0;
      int e = 0;
      size_t n = readSafe(e, rd, &ch, 1);
      if (n == 1)
      {
        if (ch == '\n')
        {
          res = 1;
          done = true;
        }
        else
        {
          o.push_back(ch);
        }
      }
      else if (e != 0)
      {
        err = e;
        done = true;
      }
      else
      {
        done = true;
      }
    }
    return res;
  }

  size_t writeLine(int &err, int wr, const std::string &s)
  {
    std::string buf = s;
    buf.push_back('\n');
    int e = 0;
    size_t n = writeSafe(e, wr, buf.data(), buf.size());
    err = e;
    if (e != 0)
    {
      return 0;
    }
    if (n == buf.size())
    {
      return 1;
    }
    return 0;
  }

  void circle(std::istream &i, std::ostream &, Maps &map)
  {
    std::string name;
    double r, x, y;
    if (!(i >> name >> r >> x >> y))
    {
      throw std::runtime_error("Invalid arguments");
    }
    if (map.shapes -> count(name))
    {
      throw std::runtime_error("Invalid name");
    }
    Circle res = {r, x, y};
    map.shapes[name] = res;
  }

  void set(std::istream &i, std::ostream &, Maps &map)
  {
    std::string name;
    size_t size;
    if (!(i >> name >> size) || size == 0)
    {
      throw std::runtime_error("Invalid arguments");
    }
    if (map.sets -> count(name))
    {
      throw std::runtime_error("Invalid name");
    }
    std::vector<Circle> shapeSet;
    shapeSet.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
      std::string shapeName;
      Circle el;
      i >> shapeName;
      try
      {
        el = map.shapes -> at(shapeName);
      }
      catch(const std::out_of_range& e)
      {
        throw std::out_of_range("No name found");
      }
      shapeSet.push_back(el);
    }
    map.sets[name] = shapeSet;
  }

  void show(std::istream &i, std::ostream &o, Maps &map)
  {
    Circle res;
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      res = map.shapes -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    o << res.r_ << res.x_ << res.y_ << "\n";
  }

  void showset(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      std::vector<Circle> &shapeSet = map.shapes -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    for (size_t i = 0; i < shapeSet.size(); ++i)
    {
      Circle res = shapeSet -> at(i);
      o << res.r_ << res.x_ << res.y_ << "\n";
    }
  }

  void frame(std::istream &i, std::ostream &o, Maps &map)
  {
    Circle res;
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      res = map.shapes -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    CircleBoundaries b = getBoundaries(res);
    o << b.maxX_ << b.maxY_ << b.minX_ << b.minY_ << "\n";
  }

  void frameset(std::istream &i, std::ostream &o, Maps &map)
  {
    Circle res;
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      std::vector<Circle> &shapeSet = map.sets -> at(name);
    }
    catch(const std::out_of_range& e)
    {
      throw std::out_of_range("No name found");
    }
    CircleBoundaries b = getSetBoundaries(shapeSet);
    o << b.maxX_ << b.maxY_ << b.minX_ << b.minY_ << "\n";
  }

  void spawn(std::istream &i, std::ostream &, Maps &map)
  {
    std::string name;
    size_t seed;
    if (!(i >> name >> seed))
    {
      throw std::runtime_error("Invalid arguments");
    }
    int toChild[2] = {}, err = pipe(toChild);
    if (err)
    {
      throw std::runtime_error("Pipe failed");
    }
    int rdC = toChild[0], wrC = toChild[1];
    int toParent[2] = {}, err = pipe(toParent);
    if (err)
    {
      close(rdC);
      close(wrC);
      throw std::runtime_error("Pipe failed");
    }
    int rdP = toParent[0], wrP = toParent[1];
    pid_t pid = fork();
    if (pid < 0)
    {
      close(rdP);
      close(rdC);
      close(wrP);
      close(wrC)
      throw std::runtime_error("Fork failed");
    }
    if (pid == 0)
    {
      err = close(wrC);
      if (err)
      {
        throw std::runtime_error("Close failed");
      }
      err = close(rdP);
      if (err)
      {
        throw std::runtime_error("Close failed");
      }
      std::mt19937_64 rng(seed);
      std::string line;
      int e = 0;
      bool reading = true;
      while (reading)
      {
        if (readLine(e, rdC, line) == 0)
        {
          reading = false;
          break;
        }
        std::istringstream iss(line);
        size_t threads = 0, tries = 0, count = 0;
        if (!(iss >> threads >> tries >> count) || count == 0 || tries == 0 || threads == 0)
        {
          int we = 0;
          writeLine(we, wrP, "0 0");
          continue;
        }

        std::vector<Circle> circles;
        circles.reserve(count);
        bool ok = true;
        for (size_t k = 0; ok && k < count; ++k)
        {
          int re = 0;
          if (readLine(re, r, line) == 0)
          {
            ok = false;
            if (re != 0) e = re;
            break;
          }
          std::istringstream circleiss(line);
          Circle c{};
          if (!(circleiss >> c.r_ >> c.x_ >> c.y_))
          {
            ok = false;
            break;
          }
          circles.push_back(c);
        }
        if (!ok)
        {
          reading = false;
          break;
        }

        CircleBoundaries b = getSetBoundaries(circles);
        std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        size_t baseSeed = rng();
        size_t hits = runUnion(circles, threads, tries, baseSeed, b);
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        double area = b.area() * static_cast<double>(hits) / static_cast<double>(tries);

        std::ostringstream os;
        os << area << ' '  << ms;
        int wr = 0;
        writeLine(wr, wrP, os.str());
      }

      close(rdC);
      close(wrP);
      _exit(0);
    }
    else
    {
      err = close(rdC);
      if (err)
      {
        throw std::runtime_error("Pipe failed");
      }
      err = close(wrP);
      if (err)
      {
        throw std::runtime_error("Pipe failed");
      }
      ProcsInfo info{pid, toChild, toParent, seed};
      maps.procs[name] = info;
    }
  }

  void area_on(std::istream &i, std::ostream &, Maps &map)
  {
    
  }
}

