#include <iostream>
#include <string>
#include <map>
#include <random>
#include <unistd.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "commands.hpp"

namespace vishnevskiy
{
  using circ_t = std::vector< Circle >;

  bool isInside(double x, double y, const Circle &c)
  {
    double rx = x - c.x_;
    double ry = y - c.y_;
    double r = c.r_;
    return (rx * rx + ry * ry) <= (r * r);
  }

  void generatePoint(std::default_random_engine &gen, const CircleBoundaries &b, double &px, double &py, bool single)
  {
    if (single)
    {
      const double gen_min = static_cast<double>(gen.min());
      const double range = static_cast<double>(gen.max()) - gen_min;
      px = b.min_x_ + (static_cast<double>(gen()) - gen_min) * ((b.max_x_ - b.min_x_) / range);
      py = b.min_y_ + (static_cast<double>(gen()) - gen_min) * ((b.max_y_ - b.min_y_) / range);
    }
    else
    {
      std::uniform_real_distribution< double > dist_x(b.min_x_, b.max_x_);
      std::uniform_real_distribution< double > dist_y(b.min_y_, b.max_y_);
      px = dist_x(gen);
      py = dist_y(gen);
    }
  }

  size_t calc(const circ_t &circles, size_t tests, size_t seed, const CircleBoundaries &b)
  {
    std::default_random_engine gen(seed);
    size_t count = 0;
    for (size_t i = 0; i < tests; ++i)
    {
      double x = 0.0;
      double y = 0.0;
      if (circles.size() == 1)
      {
        generatePoint(gen, b, x, y, true);
      }
      else
      {
        generatePoint(gen, b, x, y, false);
      }
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
    ThreadInfo *t = static_cast< ThreadInfo * >(arg);
    t->result = calc(*t->circles, t->tests, t->seed, *t->b);
    return nullptr;
  }

  size_t runCalc(const circ_t &circles, size_t threads, size_t tests, size_t seed, const CircleBoundaries &b)
  {
    size_t block = tests / threads;
    size_t remainder = tests % threads;
    std::vector< pthread_t > ids(threads);
    std::vector< ThreadInfo > tasks(threads);
    std::vector< bool > started(threads, false);

    for (size_t i = 0; i < threads; ++i)
    {
      size_t curr_tests = block;
      if (i < remainder)
      {
        curr_tests++;
      }
      tasks[i].circles = &circles;
      tasks[i].b = &b;
      tasks[i].tests = curr_tests;
      tasks[i].seed = seed + i;
      tasks[i].result = 0;
      int err = pthread_create(&ids[i], nullptr, threadStarter, &tasks[i]);
      if (err != 0)
      {
        tasks[i].result = calc(circles, curr_tests, seed + i, b);
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
    CircleBoundaries b{0, 0, 0, 0};
    b.min_x_ = c.x_ - c.r_;
    b.max_x_ = c.x_ + c.r_;
    b.min_y_ = c.y_ - c.r_;
    b.max_y_ = c.y_ + c.r_;
    return b;
  }

  CircleBoundaries getSetBoundaries(circ_t &circles)
  {
    constexpr double mx = std::numeric_limits< double >::max();
    constexpr double mn = std::numeric_limits< double >::lowest();
    CircleBoundaries b{mx, mn, mx, mn};
    for (size_t i = 0; i < circles.size(); ++i)
    {
      const Circle &c = circles[i];
      double cur_min_x = c.x_ - c.r_;
      double cur_max_x = c.x_ + c.r_;
      double cur_min_y = c.y_ - c.r_;
      double cur_max_y = c.y_ + c.r_;
      if (cur_min_x < b.min_x_)
      {
        b.min_x_ = cur_min_x;
      }
      if (cur_max_x > b.max_x_)
      {
        b.max_x_ = cur_max_x;
      }
      if (cur_min_y < b.min_y_)
      {
        b.min_y_ = cur_min_y;
      }
      if (cur_max_y > b.max_y_)
      {
        b.max_y_ = cur_max_y;
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
      err = read(rd, const_cast< char * >(b) + r, k - r);
      if (err <= 0)
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
    if (e < 0)
    {
      std::cout << e << "\n";
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
    if (name.empty() || !std::isalpha(static_cast< unsigned char >(name[0])))
    {
      throw std::runtime_error("Invalid name");
    }
    if (map.shapes.count(name))
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
    if (map.sets.count(name))
    {
      throw std::runtime_error("Invalid name");
    }
    circ_t shape_set;
    shape_set.reserve(size);
    for (size_t j = 0; j < size; ++j)
    {
      std::string shape_name;
      Circle el;
      i >> shape_name;
      try
      {
        el = map.shapes.at(shape_name);
      }
      catch (const std::out_of_range &e)
      {
        throw std::runtime_error("No name found");
      }
      shape_set.push_back(el);
    }
    std::string more;
    if (i >> more)
    {
      throw std::runtime_error("Invalid arguments");
    }
    map.sets[name] = shape_set;
  }

  void show(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    try
    {
      Circle res = map.shapes.at(name);
      o << res.r_ << " (" << res.x_ << " " << res.y_ << ")" << "\n";
    }
    catch (const std::out_of_range &e)
    {
      throw std::runtime_error("No name found");
    }
  }

  void showset(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    std::map< std::string, circ_t >::iterator it = map.sets.find(name);
    if (it == map.sets.end())
    {
      throw std::runtime_error("No name found");
    }

    const circ_t &shape_set = it->second;
    for (size_t k = 0; k < shape_set.size(); ++k)
    {
      const Circle &res = shape_set[k];
      o << res.r_ << " (" << res.x_ << " " << res.y_ << ")\n";
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
      res = map.shapes.at(name);
    }
    catch (const std::out_of_range &e)
    {
      throw std::runtime_error("No name found");
    }
    CircleBoundaries b = getBoundaries(res);
    o << "(" << b.min_x_ << " " << b.min_y_ << ") (" << b.max_x_ << " " << b.max_y_ << ")" << "\n";
  }

  void frameset(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string name;
    if (!(i >> name))
    {
      throw std::runtime_error("Invalid arguments");
    }

    std::map< std::string, circ_t >::iterator it = map.sets.find(name);
    if (it == map.sets.end())
    {
      throw std::runtime_error("No name found");
    }

    circ_t &shape_set = it->second;
    CircleBoundaries b = getSetBoundaries(shape_set);
    o << "(" << b.min_x_ << " " << b.min_y_ << ") (" << b.max_x_ << " " << b.max_y_ << ")\n";
  }

  void spawn(std::istream &i, std::ostream &, Maps &map)
  {
    std::string name;
    size_t seed;
    if (!(i >> name >> seed))
    {
      throw std::runtime_error("Invalid arguments");
    }
    if (map.procs.count(name) != 0)
    {
      throw std::runtime_error("Process already exists");
    }
    int to_child[2] = {}, err = pipe(to_child);
    if (err)
    {
      throw std::runtime_error("Pipe failed");
    }
    int rd_c = to_child[0], wr_c = to_child[1];
    int to_parent[2] = {};
    err = pipe(to_parent);
    if (err)
    {
      close(rd_c);
      close(wr_c);
      throw std::runtime_error("Pipe failed");
    }
    int rd_p = to_parent[0], wr_p = to_parent[1];
    pid_t pid = fork();
    if (pid < 0)
    {
      close(rd_p);
      close(rd_c);
      close(wr_p);
      close(wr_c);
      throw std::runtime_error("Fork failed");
    }
    if (pid == 0)
    {
      err = close(wr_c);
      if (err)
      {
        throw std::runtime_error("Close failed");
      }
      err = close(rd_p);
      if (err)
      {
        throw std::runtime_error("Close failed");
      }
      std::default_random_engine rng(seed);
      std::string line;
      int e = 0;
      bool reading = true;
      while (reading)
      {
        if (readLine(e, rd_c, line) == 0)
        {
          reading = false;
          break;
        }
        std::istringstream iss(line);
        size_t threads = 0, tries = 0, count = 0;
        if (!(iss >> threads >> tries >> count) || count == 0 || tries == 0 || threads == 0)
        {
          int we = 0;
          writeLine(we, wr_p, "0 0");
          continue;
        }

        circ_t circles;
        circles.reserve(count);
        bool ok = true;
        for (size_t k = 0; ok && k < count; ++k)
        {
          int re = 0;
          if (readLine(re, rd_c, line) == 0)
          {
            ok = false;
            if (re != 0)
            {
              e = re;
            }
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
        size_t hits = runCalc(circles, threads, tries, seed, b);
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        double ms = std::chrono::duration< double, std::milli >(t1 - t0).count();
        double area = b.area() * static_cast< double >(hits) / static_cast< double >(tries);

        std::ostringstream os;
        os << std::fixed << std::setprecision(3) << area << ' ' << std::fixed << std::setprecision(3) << ms;
        int wr = 0;
        writeLine(wr, wr_p, os.str());
      }

      close(rd_c);
      close(wr_p);
      _exit(0);
    }
    else
    {
      err = close(rd_c);
      if (err)
      {
        throw std::runtime_error("Pipe failed");
      }
      err = close(wr_p);
      if (err)
      {
        throw std::runtime_error("Pipe failed");
      }
      ProcInfo info{pid, wr_c, rd_p, seed};
      map.procs[name] = info;
    }
  }

  void area_on(std::istream &i, std::ostream &, Maps &map)
  {
    std::string proc_name, calc_name, set_name;
    long long threads = 0, tries = 0;
    if (!(i >> proc_name >> calc_name >> set_name >> threads >> tries))
    {
      throw std::runtime_error("Invalid arguments");
    }
    std::map< std::string, ProcInfo >::iterator pit = map.procs.find(proc_name);
    if (pit == map.procs.end())
    {
      throw std::runtime_error("Invalid process");
    }
    if (map.calcs.count(calc_name))
    {
      throw std::runtime_error("Invalid calc name");
    }
    std::map< std::string, circ_t >::iterator sit = map.sets.find(set_name);
    if (sit == map.sets.end())
    {
      throw std::runtime_error("Invalid set");
    }
    if (threads <= 0 || tries <= 0)
    {
      throw std::runtime_error("Invalid arguments");
    }

    const circ_t &circles = sit->second;
    std::ostringstream oss;
    oss << threads << ' ' << tries << ' ' << circles.size();
    int we = 0;
    if (writeLine(we, pit->second.pipe_to_child, oss.str()) == 0)
    {
      throw std::runtime_error("Write failed");
    }

    for (size_t k = 0; k < circles.size(); ++k)
    {
      std::ostringstream os;
      os << circles[k].r_ << ' ' << circles[k].x_ << ' ' << circles[k].y_;
      if (writeLine(we, pit->second.pipe_to_child, os.str()) == 0)
      {
        throw std::runtime_error("Write failed");
      }
    }
    CalcInfo info{proc_name, false, 0.0, 0.0};
    map.calcs[calc_name] = info;
  }

  void status(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string calc_name;
    if (!(i >> calc_name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    std::map< std::string, CalcInfo >::iterator it = map.calcs.find(calc_name);
    if (it == map.calcs.end())
    {
      throw std::runtime_error("Invalid calc");
    }
    const CalcInfo &c = it->second;
    if (!c.done)
    {
      o << c.proc_name << " <IN PROGRESS>\n";
    }
    else
    {
      o << c.proc_name << ' ' << std::fixed << std::setprecision(3) << c.ms;
      o << ' ' << std::fixed << std::setprecision(3) << c.area << '\n';
    }
  }

  void wait(std::istream &i, std::ostream &o, Maps &map)
  {
    std::string calc_name;
    if (!(i >> calc_name))
    {
      throw std::runtime_error("Invalid arguments");
    }
    std::map< std::string, CalcInfo >::iterator it = map.calcs.find(calc_name);
    if (it == map.calcs.end())
    {
      throw std::runtime_error("Invalid calc");
    }
    CalcInfo &c = it->second;
    if (!c.done)
    {
      std::map< std::string, ProcInfo >::iterator pit = map.procs.find(c.proc_name);
      if (pit == map.procs.end())
      {
        throw std::runtime_error("Invalid process");
      }
      std::string line;
      int e = 0;
      if (readLine(e, pit->second.pipe_to_parent, line) == 0)
      {
        throw std::runtime_error("Read failed");
      }
      std::istringstream is(line);
      double area = 0.0, ms = 0.0;
      if (!(is >> area >> ms))
      {
        throw std::runtime_error("Invalid response");
      }
      c.area = area;
      c.ms = ms;
      c.done = true;
    }
    o << c.proc_name << ' ' << std::fixed << std::setprecision(3) << c.ms;
    o << ' ' << std::fixed << std::setprecision(3) << c.area << '\n';
  }
}

